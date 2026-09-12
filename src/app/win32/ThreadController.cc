#include "pch.h"
#include "ThreadController.h"

namespace {
inline constexpr winrt::hresult kErrorTooManyThreads = winrt::impl::hresult_from_win32(ERROR_TOO_MANY_THREADS);
inline constexpr winrt::hresult kErrorInvalidOperation = winrt::impl::hresult_from_win32(ERROR_INVALID_OPERATION);
}

using namespace roxyg::win32;

ThreadController::ThreadController() noexcept
  : mutex_()
  , state_({INVALID_HANDLE_VALUE, 0}) {
}

#if _DEBUG
ThreadController::~ThreadController() noexcept {
  assert(hThread() == INVALID_HANDLE_VALUE);
}
#endif

winrt::hresult ThreadController::start(_beginthreadex_proc_type proc, void* params) noexcept {
  if (proc == nullptr) {
    return E_INVALIDARG;
  }

  std::lock_guard<std::mutex> lock(mutex_);

  HANDLE const current_hthread{hThread()};
  if (current_hthread != INVALID_HANDLE_VALUE) {
    return kErrorInvalidOperation;
  }

  unsigned int thread_id = 0;
  HANDLE const hthread = reinterpret_cast<HANDLE>(_beginthreadex(
    nullptr,
    0,
    proc,
    params,
    0,
    &thread_id
    ));
  if (hthread == nullptr) {
    switch (errno) {
    case EINVAL:
      return E_INVALIDARG;
    case EACCES:
      return E_OUTOFMEMORY;
    case EAGAIN:
      return kErrorTooManyThreads;
    default:
      return E_FAIL;
    }
  }

  state_.store({hthread, thread_id}, std::memory_order_release);
  return S_OK;
}

DWORD ThreadController::validateThreadAccess(ThreadState const& state) noexcept {
  if (INVALID_HANDLE_VALUE == state.hthread) {
    return ERROR_INVALID_OPERATION;
  }
  if (GetCurrentThreadId() == state.thread_id) {
    return ERROR_POSSIBLE_DEADLOCK;
  }
  return ERROR_SUCCESS;
}

DWORD ThreadController::reapThread(HANDLE hthread) noexcept {
  DWORD status = ERROR_SUCCESS;
  BOOL rc = GetExitCodeThread(hthread, &status);
  if (rc == FALSE) {
    status = WINRT_IMPL_GetLastError();
  }

  rc = CloseHandle(hthread);
  if (rc == FALSE) {
    return WINRT_IMPL_GetLastError();
  }

  state_.store({INVALID_HANDLE_VALUE, 0}, std::memory_order_release);
  return status;
}
