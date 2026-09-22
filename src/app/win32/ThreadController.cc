#include "pch.h"
#include "ThreadController.h"

#include "../win32/hresult.h"

using namespace roxyg::win32;

ThreadController::ThreadController() noexcept
  : mutex_()
  , data_({INVALID_HANDLE_VALUE, 0})
  , state_(State::kReady) {
}

ThreadController::~ThreadController() noexcept {
#if _DEBUG
  assert(hThread() == INVALID_HANDLE_VALUE);
#endif
}

winrt::hresult ThreadController::start(_beginthreadex_proc_type proc, void* params, ThreadInfo* info) noexcept {
  if (proc == nullptr) {
    return E_INVALIDARG;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kReady) {
    return hresult::kErrorInvalidOperation;
  }

  HANDLE const current_hthread{hThread()};
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
    return winrt::impl::hresult_from_win32(status);
  }

  HANDLE const hthread = reinterpret_cast<HANDLE>(raw_thread);
  data_.store({hthread, thread_id}, std::memory_order_release);
  state_ = State::kRunning;

  if (info) {
    *info = {hthread, thread_id};
  }
  return S_OK;
}

DWORD ThreadController::validateThreadAccess(ThreadInfo const& state) noexcept {
  if (INVALID_HANDLE_VALUE == state.hthread) {
    return ERROR_INVALID_OPERATION;
  }
  if (GetCurrentThreadId() == state.thread_id) {
    return ERROR_POSSIBLE_DEADLOCK;
  }
  return ERROR_SUCCESS;
}

winrt::hresult ThreadController::reapThread(HANDLE hthread) noexcept {
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

winrt::hresult ThreadController::forceExitThread(HANDLE hthread) noexcept {
#if _DEBUG
  assert(state_ == State::kStopping);
#endif

  DWORD status = ERROR_SUCCESS;
#pragma warning(push)
#pragma warning(disable:6258)
  BOOL rc = TerminateThread(hthread, 1);
#pragma warning(pop)
  if (rc == FALSE) {
    status = WINRT_IMPL_GetLastError();
  }

  DWORD const wait_status = WaitForSingleObject(hthread, INFINITE);
  if (wait_status == WAIT_FAILED) {
    status = WINRT_IMPL_GetLastError();
  }

  rc = CloseHandle(hthread);
  if (rc == FALSE) {
    status = WINRT_IMPL_GetLastError();
  }

  data_.store({INVALID_HANDLE_VALUE, 0}, std::memory_order_release);
  state_ = State::kReady;
  return hresult::HResultFromWin32(status);
}
