#include "pch.h"
#include "WinEventHookController.h"
#include "ThreadContext.h"

#include "../utility/backoff/fixed.h"
#include "../utility/backoff/exponential.h"
#include "../utility/fastfail.h"
#include "../utility/RetryState.inl"
#include "../win32/hresult.h"

namespace {

inline constexpr std::wstring_view kWinEventHookRetryFactoryAllocationFailed
  = L"Failed to allocate memory for the WinEvent hook retry state factory.";
inline constexpr std::wstring_view kWinEventHookRetryStateAllocationFailed
  = L"Failed to allocate memory for the WinEvent hook retry state.";
inline constexpr std::wstring_view kPostWinEventHookThreadQuitMessageFailedQuota
  = L"Failed to post a quit message to the WinEvent hook thread. Not enough quota is available.";
inline constexpr std::wstring_view kPostWinEventHookThreadQuitMessageFailed
  = L"Failed to post a quit message to the WinEvent hook thread.";
inline constexpr std::wstring_view kWaitForWinEventHookThreadExitFailed
  = L"Failed to wait for the WinEvent hook thread to exit.";
inline constexpr std::wstring_view kWaitForWinEventHookThreadExitTimeout
  = L"The wait for the WinEvent hook thread to exit timed out.";
inline constexpr std::wstring_view kWinEventHookThreadExitRetryExpired
  = L"The WinEvent hook thread did not exit within the retry limit.";

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
  : ThreadController(ThreadStopFailurePolicy::kFailFast)
  , retry_factory_(std::move(retry_factory)) {
}

WinEventHookController::WinEventHookController() noexcept
  : WinEventHookController(std::make_unique<utility::RetryStateFactory<1, utility::uint32_fixed_backoff<1000>>>()) {
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
  winrt::hresult hr = ThreadController::start(WinEventHookWorker, &state, &info);
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

winrt::hresult WinEventHookController::stop() noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kRunning && state_ != State::kStopping) {
    return hresult::kErrorInvalidOperation;
  }

  ThreadInfo const current_info{threadInfo()};
  {
    winrt::hresult const hr = validateThreadAccess(current_info);
    if (FAILED(hr)) {
      return hr;
    }
  }

  // Record the stopping state before requesting thread exit.
  state_ = State::kStopping;

  std::unique_ptr<utility::IRetryState> retry;
  try {
    retry = retry_factory_->make();
  } catch (std::bad_alloc const&) {
    logger_.fatal(winrt::hstring{kWinEventHookRetryStateAllocationFailed}, E_OUTOFMEMORY);
    utility::fastfail();
  }

  do {
    BOOL rc = PostThreadMessageW(current_info.thread_id, WM_QUIT, 0, 0);
    bool retryable = true;
    DWORD delay;
    if (rc == FALSE) {
      DWORD const lasterr = WINRT_IMPL_GetLastError();
      if (lasterr == ERROR_NOT_ENOUGH_QUOTA) {
        delay = retry->nextDelay();
        logger_.notice(winrt::hstring{kPostWinEventHookThreadQuitMessageFailedQuota}, hresult::kErrorNotEnoughQuota);
      } else {
        retryable = false;
        delay = 0;  // force immediate return
        logger_.error(winrt::hstring{kPostWinEventHookThreadQuitMessageFailed}, hresult::HResultFromWin32(lasterr));
      }
    } else {
      delay = retry->nextDelay();
    }

    DWORD const wait_status = WaitForSingleObject(current_info.hthread, delay);
    if (wait_status == WAIT_OBJECT_0) {
      break;
    } else {
      if (wait_status == WAIT_TIMEOUT) {
        logger_.error(winrt::hstring{kWaitForWinEventHookThreadExitTimeout}, hresult::kErrorTimeout);
      } else {
        winrt::hresult hr = hresult::LastErrorAsHResult();
        logger_.error(winrt::hstring{kWaitForWinEventHookThreadExitFailed}, hr);
      }
      if (retryable) {
        retry->advanceAttempt();
      } else {
        retry->forceExpire();
        break;
      }
    }
  } while (retry->available());

  if (retry->expired()) {
    if (stop_failure_policy_ == ThreadStopFailurePolicy::kFailFast) {
      logger_.fatal(winrt::hstring{kWinEventHookThreadExitRetryExpired}, hresult::kErrorTimeout);
      utility::fastfail();
    }

    return hresult::kErrorTimeout;
  }

  return reapThread(current_info.hthread);
}
