#include "pch.h"
#include "WinEventHookController.h"
#include "ThreadContext.h"

#include "../utility/backoff/exponential.h"
#include "../utility/fastfail.h"
#include "../utility/RetryState.inl"
#include "../win32/hresult.h"

namespace {

inline constexpr std::wstring_view kWinEventHookRetryFactoryAllocationFailed
  = L"Failed to allocate memory for the WinEvent hook retry state factory.";

}

using WinEventHookExitBackoff = roxyg::utility::uint32_exponential_backoff<
  1000, 10000,
  2.f,
  roxyg::utility::preferred_xorshift_equal_jitter_generator
>;

using namespace roxyg::win32;

struct WinEventHookThreadContext final: public ThreadContext {
  DWORD const event_min, event_max;
  WINEVENTPROC const wndproc;
};

static unsigned int __stdcall WinEventHookWorker(void* p) noexcept {
  WinEventHookThreadContext& ctx = *static_cast<WinEventHookThreadContext*>(p);

  HWINEVENTHOOK hWinEventHook = SetWinEventHook(
    ctx.event_min, ctx.event_max,
    nullptr,
    ctx.wndproc,
    0, 0,
    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (!hWinEventHook) {
    ctx.notifyLastError();
    return 0;
  }
  ctx.notify(S_OK);

  winrt::hresult hr = S_OK;
  BOOL rc;
  MSG msg;
  while ((rc = GetMessageW(&msg, nullptr, 0, 0)) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
  if (rc == -1) {
    hr = hresult::LastErrorAsHResult();
  }

  rc = UnhookWinEvent(hWinEventHook);
  if (rc == FALSE && hr == S_OK) {
    hr = E_FAIL;
  }

  _endthreadex(hr);
  return 0;
}

WinEventHookController::WinEventHookController(std::unique_ptr<utility::IRetryStateFactory>&& retry_factory) noexcept
  : MessageLoopThreadController(ThreadStopFailurePolicy::kFailFast, std::move(retry_factory)) {
}

WinEventHookController::WinEventHookController() noexcept
  : MessageLoopThreadController() {
}

winrt::hresult WinEventHookController::start(
  DWORD event_min,
  DWORD event_max,
  WINEVENTPROC wndproc
) noexcept {
  WinEventHookThreadContext state{
    .event_min = event_min,
    .event_max = event_max,
    .wndproc = wndproc,
  };
  ThreadInfo info;
  winrt::hresult hr = MessageLoopThreadController::start(WinEventHookWorker, &state, &info);
  if (FAILED(hr)) {
    return hr;
  }

  hr = state.wait_and_load();
  if (hr != S_OK) {
    [[maybe_unused]] DWORD const stop_status = stop();
  } else {
    size_t const seed = utility::make_preferred_seed(utility::get_tsc(), info.thread_id);
    try {
      std::unique_ptr<utility::IRetryStateFactory> state_factory{
        std::make_unique<utility::RetryStateFactory<5, WinEventHookExitBackoff>>(WinEventHookExitBackoff{{seed}})
      };
      setRetryFactory(std::move(state_factory));
    } catch (std::bad_alloc const&) {
      // Fail fast here because stop() may be unable to allocate its retry state
      // after this allocation failure, leaving the worker thread unrecoverable.
      logger_.fatal(winrt::hstring{kWinEventHookRetryFactoryAllocationFailed}, E_OUTOFMEMORY);
      utility::fastfail();
    }
  }
  return hr;
}

BOOL WinEventHookController::postStopMessage(intptr_t target) noexcept {
  return PostThreadMessageW(static_cast<DWORD>(target), WM_QUIT, 0, 0);
}

winrt::hresult WinEventHookController::stop() noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kRunning && state_ != State::kStopping) {
    return hresult::kErrorInvalidOperation;
  }

  ThreadInfo const current_info{threadInfo()};
  winrt::hresult const hr = validateThreadAccess(current_info);
  if (FAILED(hr)) {
    return hr;
  }

  // Record the stopping state before requesting thread exit.
  state_ = State::kStopping;

  return stopThread(current_info.hthread, static_cast<intptr_t>(current_info.thread_id));
}
