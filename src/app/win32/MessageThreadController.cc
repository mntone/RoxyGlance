#include "pch.h"
#include "MessageThreadController.h"

namespace {
inline constexpr winrt::hresult kErrorTooManyThreads = winrt::impl::hresult_from_win32(ERROR_TOO_MANY_THREADS);
inline constexpr winrt::hresult kErrorInvalidOperation = winrt::impl::hresult_from_win32(ERROR_INVALID_OPERATION);
}

using namespace roxyg::win32;

MessageThreadController::MessageThreadController() noexcept
  : mutex_()
  , state_({INVALID_HANDLE_VALUE, 0}) {
}

#if _DEBUG
MessageThreadController::~MessageThreadController() noexcept {
  assert(hThread() == INVALID_HANDLE_VALUE);
}
#endif

winrt::hresult MessageThreadController::start(_beginthreadex_proc_type proc, void* params) noexcept {
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

DWORD MessageThreadController::stop(DWORD timeout) noexcept {
  std::lock_guard<std::mutex> lock(mutex_);

  ThreadState const current_state{threadState()};
  if (INVALID_HANDLE_VALUE == current_state.hthread) {
    return ERROR_INVALID_OPERATION;
  }
  if (GetCurrentThreadId() == current_state.thread_id) {
    return ERROR_POSSIBLE_DEADLOCK;
  }

  DWORD wait_status = WAIT_OBJECT_0;
  BOOL rc = PostThreadMessageW(current_state.thread_id, WM_QUIT, 0, 0);
  if (rc == FALSE) {
    DWORD const post_message_status = WINRT_IMPL_GetLastError();
    wait_status = WaitForSingleObject(current_state.hthread, 0);
    switch (wait_status) {
    case WAIT_TIMEOUT:
      return post_message_status;
    case WAIT_FAILED:
      return WINRT_IMPL_GetLastError();
    }
    goto finalize;
  }

  wait_status = WaitForSingleObject(current_state.hthread, timeout);
  switch (wait_status) {
  case WAIT_TIMEOUT:
    return ERROR_TIMEOUT;
  case WAIT_FAILED:
    return WINRT_IMPL_GetLastError();
  }

finalize:
  DWORD status = ERROR_SUCCESS;
  rc = GetExitCodeThread(current_state.hthread, &status);
  if (rc == FALSE) {
    status = WINRT_IMPL_GetLastError();
  }

  rc = CloseHandle(current_state.hthread);
  if (rc == FALSE) {
    return WINRT_IMPL_GetLastError();
  }

  state_.store({INVALID_HANDLE_VALUE, 0}, std::memory_order_release);
  return status;
}
