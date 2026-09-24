#include "pch.h"
#include "WindowController.h"
#include "ThreadContext.h"

#include "../utility/backoff/exponential.h"
#include "../utility/backoff/fixed.h"
#include "../utility/fastfail.h"
#include "../utility/RetryState.inl"
#include "../win32/hresult.h"

namespace {

inline constexpr wchar_t kWindowControllerThreadName[] = L"WindowController Thread";

inline constexpr std::wstring_view kWindowControllerRetryFactoryAllocationFailed
  = L"Failed to allocate memory for the WindowController retry state factory.";

}

using WinEventHookExitBackoff = roxyg::utility::uint32_exponential_backoff<
  1000, 10000,
  2.f,
  roxyg::utility::preferred_xorshift_equal_jitter_generator
>;

using namespace roxyg::win32;

HINSTANCE WindowController::hinstance_{nullptr};

static LRESULT __stdcall WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  if (message == WM_CREATE) {
    // SetWindowLongPtrW does not reset the last-error code on success.
    SetLastError(ERROR_SUCCESS);

    LPVOID controller_pointer = reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams;
    LONG_PTR rc = SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(controller_pointer));
    if (rc == 0) {
      uint32_t const lasterr = WINRT_IMPL_GetLastError();
      if (lasterr != ERROR_SUCCESS) {
        winrt::throw_last_error();
      }
    }

    WindowController& controller = *static_cast<WindowController*>(controller_pointer);
    return controller.windowProc(hwnd, message, wparam, lparam);
  } else {
    LONG_PTR controller_pointer = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (controller_pointer) {
      WindowController& controller = *reinterpret_cast<WindowController*>(controller_pointer);
      return controller.windowProc(hwnd, message, wparam, lparam);
    } else {
      return DefWindowProcW(hwnd, message, wparam, lparam);
    }
  }
}

struct WindowThreadContext final: public ThreadContext {
  std::atomic<HWND> atomic_hwnd;
  HINSTANCE const hinstance;
  wchar_t const* const class_name;
  void* const controller;

  ROXYG_ALWAYS_INLINE HWND hWnd() const noexcept {
    return atomic_hwnd.load(std::memory_order_acquire);
  }

  ROXYG_ALWAYS_INLINE void setHwnd(HWND value) noexcept {
    atomic_hwnd.store(value, std::memory_order_release);
  }
};

static unsigned int __stdcall WindowWorker(void* p) noexcept {
  WindowThreadContext& ctx = *static_cast<WindowThreadContext*>(p);

  WNDCLASSEXW wcex{
    .cbSize = sizeof(WNDCLASSEXW),
    .style = 0,
    .lpfnWndProc = WindowProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = ctx.hinstance,
    .hIcon = nullptr,
    .hCursor = LoadCursorW(nullptr, IDC_ARROW),
    .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
    .lpszMenuName = nullptr,
    .lpszClassName = ctx.class_name,
    .hIconSm = nullptr,
  };
  if (RegisterClassExW(&wcex) == 0) {
    return ctx.notifyLastError();
  }

  HWND const hwnd = CreateWindowExW(
    0,
    ctx.class_name,
    L"",
    0,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
    HWND_MESSAGE,
    nullptr,
    ctx.hinstance,
    ctx.controller
  );
  if (!hwnd) {
    winrt::hresult const hr{hresult::LastErrorAsHResult()};
    [[maybe_unused]] BOOL const rc = UnregisterClassW(ctx.class_name, ctx.hinstance);
    ctx.notify(hr);
    _endthreadex(EXIT_FAILURE);
    return 0;
  }

  MSG msg;
  [[maybe_unused]] BOOL const peek_result = PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
  ctx.setHwnd(hwnd);
  ctx.notify(S_OK);

  winrt::hresult hr = S_OK;
  BOOL rc;
  while ((rc = GetMessageW(&msg, nullptr, 0, 0)) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  if (rc == -1) {
    hr = hresult::LastErrorAsHResult();
  } else if (rc == 0 && msg.message == WM_QUIT) {
    hr = static_cast<HRESULT>(msg.wParam);
  }

  rc = UnregisterClassW(wcex.lpszClassName, wcex.hInstance);
  if (rc == FALSE && hr == S_OK) {
    hr = hresult::LastErrorAsHResult();
  }

  _endthreadex(hr);
  return 0;
}

WindowController::WindowController() noexcept
  : MessageLoopThreadController(
      ThreadStopFailurePolicy::kFailFast,
      std::make_unique<utility::RetryStateFactory<1, utility::uint32_fixed_backoff<1000>>>()
    )
  , hwnd_(nullptr) {
}

winrt::hresult WindowController::start(wchar_t const* class_name) noexcept {
  HINSTANCE hinstance = hinstance_;
  if (!hinstance) {
    hinstance = GetModuleHandleW(nullptr);
    if (!hinstance) {
      return hresult::LastErrorAsHResult();
    }
    hinstance_ = hinstance;
  }

  WindowThreadContext state{
    .atomic_hwnd = nullptr,
    .hinstance = hinstance,
    .class_name = class_name,
    .controller = this,
  };
  ThreadInfo info;
  winrt::hresult hr = MessageLoopThreadController::start(WindowWorker, &state, &info);
  if (FAILED(hr)) {
    return hr;
  }

  hr = state.wait_and_load();
  if (hr != S_OK) {
    // The worker thread already exited on its own; reap it without posting a stop message.
    [[maybe_unused]] winrt::hresult const recover_hr = reapThreadAfterStartFailure(info.hthread);
    return hr;
  }

  size_t const seed = utility::make_preferred_seed(utility::get_tsc(), info.thread_id);
  try {
    std::unique_ptr<utility::IRetryStateFactory> state_factory{
      std::make_unique<utility::RetryStateFactory<5, WinEventHookExitBackoff>>(WinEventHookExitBackoff{{seed}})
    };
    setRetryFactory(std::move(state_factory));
  } catch (std::bad_alloc const&) {
    // Fail fast here because stop() may be unable to allocate its retry state
    // after this allocation failure, leaving the worker thread unrecoverable.
    logger_.fatal(winrt::hstring{kWindowControllerRetryFactoryAllocationFailed}, E_OUTOFMEMORY);
    utility::fastfail();
  }

#if _DEBUG
  SetThreadDescription(info.hthread, kWindowControllerThreadName);
#endif

  hwnd_.store(state.hWnd(), std::memory_order_release);
  return S_OK;
}

BOOL WindowController::postStopMessage(intptr_t target) noexcept {
  return PostMessageW(reinterpret_cast<HWND>(target), WM_CLOSE, 0, 0);
}

winrt::hresult WindowController::stop() noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kRunning && state_ != State::kStopping) {
    return hresult::kErrorInvalidOperation;
  }

  ThreadInfo const current_info{threadInfo()};
  winrt::hresult hr = validateThreadAccess(current_info);
  if (FAILED(hr)) {
    return hr;
  }

  HWND const hwnd = hwnd_.load(std::memory_order_acquire);
  if (hwnd == nullptr) {
    return hresult::kErrorInvalidOperation;
  }

  // Record the stopping state before requesting thread exit.
  state_ = State::kStopping;

  hr = stopThread(current_info.hthread, reinterpret_cast<intptr_t>(hwnd));
  if (FAILED(hr)) {
    return hr;
  }

  hwnd_.store(nullptr, std::memory_order_release);
  return hr;
}
