#include "pch.h"
#include "MessageLoopThreadController.h"

#include "../utility/backoff/fixed.h"
#include "../utility/fastfail.h"
#include "../utility/RetryState.inl"
#include "../win32/hresult.h"

namespace {

inline constexpr DWORD kStartFailureReapTimeoutMs = 1000;

inline constexpr std::wstring_view kThreadDidNotExitAfterStartFailure
  = L"Worker thread did not exit after a start() failure.";
inline constexpr std::wstring_view kThreadNotReapedBeforeDestruction
  = L"Message loop thread was not reaped before destruction.";
inline constexpr std::wstring_view kRetryStateAllocationFailed
  = L"Failed to allocate memory for the thread stop retry state.";
inline constexpr std::wstring_view kPostStopMessageFailedQuota
  = L"Failed to post a stop message to the thread. Not enough quota is available.";
inline constexpr std::wstring_view kPostStopMessageFailed
  = L"Failed to post a stop message to the thread.";
inline constexpr std::wstring_view kWaitForThreadExitFailed
  = L"Failed to wait for the thread to exit.";
inline constexpr std::wstring_view kWaitForThreadExitTimeout
  = L"The wait for the thread to exit timed out.";
inline constexpr std::wstring_view kThreadExitRetryExpired
  = L"The thread did not exit within the retry limit.";

}

using namespace roxyg::win32;

MessageLoopThreadController::MessageLoopThreadController(
  ThreadStopFailurePolicy stop_failure_policy,
  std::unique_ptr<utility::IRetryStateFactory>&& retry_factory
) noexcept
  : mutex_()
  , data_({INVALID_HANDLE_VALUE, 0})
  , retry_factory_(std::move(retry_factory))
  , logger_()
  , state_(State::kReady)
  , stop_failure_policy_(stop_failure_policy) {
}

MessageLoopThreadController::MessageLoopThreadController(ThreadStopFailurePolicy stop_failure_policy) noexcept
  : MessageLoopThreadController(
    stop_failure_policy,
    std::make_unique<utility::RetryStateFactory<1, utility::uint32_fixed_backoff<1000>>>()
  ) {
}

MessageLoopThreadController::MessageLoopThreadController() noexcept
  : MessageLoopThreadController(
    ThreadStopFailurePolicy::kReturnTimeout,
    std::make_unique<utility::RetryStateFactory<1, utility::uint32_fixed_backoff<1000>>>()
  ) {
}

MessageLoopThreadController::~MessageLoopThreadController() noexcept {
  HANDLE const hthread{threadInfo().hthread};
  if (hthread == INVALID_HANDLE_VALUE) {
    return;
  }

  logger_.fatal(winrt::hstring{kThreadNotReapedBeforeDestruction}, hresult::kErrorInvalidOperation);
#if _DEBUG
  assert(hthread == INVALID_HANDLE_VALUE);
#endif
  utility::fastfail();
}

winrt::hresult MessageLoopThreadController::start(_beginthreadex_proc_type proc, void* params, ThreadInfo* info) noexcept {
  if (proc == nullptr) {
    return E_INVALIDARG;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kReady) {
    return hresult::kErrorInvalidOperation;
  }

  HANDLE const current_hthread{threadInfo().hthread};
  if (current_hthread != INVALID_HANDLE_VALUE) {
    return hresult::kErrorInvalidOperation;
  }

  // Clear the CRT error indicators before calling _beginthreadex
  // to avoid using stale values if the function does not update them.
  _set_doserrno(0);
  errno = 0;

  unsigned int thread_id;
  uintptr_t const raw_thread = _beginthreadex(
    nullptr,
    0,
    proc,
    params,
    0,
    &thread_id
    );
  if (raw_thread == 0) {
    DWORD status = _doserrno;
    if (status == ERROR_SUCCESS) {
      switch (errno) {
      case EINVAL:
        status = ERROR_INVALID_PARAMETER;
        break;
      case EAGAIN:
        status = ERROR_TOO_MANY_THREADS;
        break;
      case EACCES:
        status = ERROR_ACCESS_DENIED;
        break;
      default:
        status = ERROR_FUNCTION_FAILED;
        break;
      }
    }
    return hresult::HResultFromWin32(status);
  }

  HANDLE const hthread = reinterpret_cast<HANDLE>(raw_thread);
  data_.store({hthread, thread_id}, std::memory_order_release);
  state_ = State::kRunning;

  if (info) {
    *info = {hthread, thread_id};
  }
  return S_OK;
}

winrt::hresult MessageLoopThreadController::reapThreadAfterStartFailure(HANDLE hthread) noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
#if _DEBUG
  assert(state_ == State::kRunning);
#endif
  state_ = State::kStopping;

  // The thread already exited (or is about to) on its own, so a single bounded wait
  // suffices; there is no stop message to redeliver by retrying.
  DWORD const wait_status = WaitForSingleObject(hthread, kStartFailureReapTimeoutMs);
  if (wait_status != WAIT_OBJECT_0) {
    winrt::hresult hr;
    if (wait_status == WAIT_TIMEOUT) {
      logger_.error(winrt::hstring{kWaitForThreadExitTimeout}, hresult::kErrorTimeout);
      hr = hresult::kErrorTimeout;
    } else {
      hr = hresult::LastErrorAsHResult();
      logger_.error(winrt::hstring{kWaitForThreadExitFailed}, hr);
    }

    if (stop_failure_policy_ == ThreadStopFailurePolicy::kFailFast) {
      logger_.fatal(winrt::hstring{kThreadDidNotExitAfterStartFailure}, hr);
      utility::fastfail();
    }
    return hr;
  }

  return reapThread(hthread);
}

winrt::hresult MessageLoopThreadController::validateThreadAccess(ThreadInfo const& state) noexcept {
  DWORD err;
  if (INVALID_HANDLE_VALUE == state.hthread) {
    err = ERROR_INVALID_OPERATION;
  } else if (GetCurrentThreadId() == state.thread_id) {
    err = ERROR_POSSIBLE_DEADLOCK;
  } else {
    err = ERROR_SUCCESS;
  }
  return hresult::HResultFromWin32(err);
}

winrt::hresult MessageLoopThreadController::stopThread(HANDLE const hthread, intptr_t target) noexcept {
#if _DEBUG
  assert(state_ == State::kStopping);
#endif

  std::unique_ptr<utility::IRetryState> retry;
  try {
    retry = retry_factory_->make();
  } catch (std::bad_alloc const&) {
    logger_.fatal(winrt::hstring{kRetryStateAllocationFailed}, E_OUTOFMEMORY);
    utility::fastfail();
  }

  winrt::hresult hr;
  do {
    hr = S_OK;

    BOOL rc = postStopMessage(target);
    bool retryable = true;
    DWORD delay;
    if (rc == FALSE) {
      DWORD const lasterr = WINRT_IMPL_GetLastError();
      if (lasterr == ERROR_NOT_ENOUGH_QUOTA) {
        delay = retry->nextDelay();
        hr = hresult::kErrorNotEnoughQuota;
        logger_.notice(winrt::hstring{kPostStopMessageFailedQuota}, hresult::kErrorNotEnoughQuota);
      } else {
        retryable = false;
        delay = 0;  // force immediate return
        hr = hresult::HResultFromWin32(lasterr);
        logger_.error(winrt::hstring{kPostStopMessageFailed}, hr);
      }
    } else {
      delay = retry->nextDelay();
    }

    DWORD const wait_status = WaitForSingleObject(hthread, delay);
    if (wait_status == WAIT_OBJECT_0) {
      hr = S_OK;
      break;
    } else {
      if (wait_status == WAIT_TIMEOUT) {
        logger_.error(winrt::hstring{kWaitForThreadExitTimeout}, hresult::kErrorTimeout);
        if (hr == S_OK) {
          hr = hresult::kErrorTimeout;
        }
      } else {
        winrt::hresult const wait_hr = hresult::LastErrorAsHResult();
        logger_.error(winrt::hstring{kWaitForThreadExitFailed}, wait_hr);
        if (hr == S_OK) {
          hr = wait_hr;
        }
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
      logger_.fatal(winrt::hstring{kThreadExitRetryExpired}, hr);
      utility::fastfail();
    }

    return hr;
  }

  return reapThread(hthread);
}

winrt::hresult MessageLoopThreadController::reapThread(HANDLE hthread) noexcept {
#if _DEBUG
  assert(state_ == State::kStopping);
#endif

  DWORD exit_code;
  BOOL rc = GetExitCodeThread(hthread, &exit_code);

  winrt::hresult hr;
  if (rc == FALSE) {
    hr = hresult::LastErrorAsHResult();
  } else {
    hr = static_cast<HRESULT>(exit_code);
  }

  rc = CloseHandle(hthread);
  if (rc == FALSE) {
    return hresult::LastErrorAsHResult();
  }

  data_.store({INVALID_HANDLE_VALUE, 0}, std::memory_order_release);
  state_ = State::kReady;
  return hr;
}
