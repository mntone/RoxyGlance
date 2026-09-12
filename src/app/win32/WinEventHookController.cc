#include "pch.h"
#include "WinEventHookController.h"

struct WinEventHookThreadParams final {
  const DWORD event_min, event_max;
  const WINEVENTPROC proc;
  std::atomic<DWORD> status;
  std::atomic<bool> ready;
};

static __forceinline void notifyBeginThread(WinEventHookThreadParams& params) noexcept {
  params.ready.store(true, std::memory_order_release);
  params.ready.notify_one();
}

static unsigned int __stdcall WinEventHookWorker(void* p) noexcept {
  WinEventHookThreadParams& params = *static_cast<WinEventHookThreadParams*>(p);
  HWINEVENTHOOK hWinEventHook = SetWinEventHook(
    params.event_min, params.event_max,
    nullptr,
    params.proc,
    0, 0,
    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (!hWinEventHook) {
    params.status.store(WINRT_IMPL_GetLastError(), std::memory_order_relaxed);
    notifyBeginThread(params);
    _endthreadex(EXIT_FAILURE);
    return 0;
  }

  MSG msg;
  [[maybe_unused]] BOOL const peek_result = PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
  notifyBeginThread(params);

  DWORD ret = EXIT_SUCCESS;
  BOOL rc;
  while ((rc = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
    if (rc == -1) {
      ret = EXIT_FAILURE;  // invalid message pointer
      break;
    }
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  rc = UnhookWinEvent(hWinEventHook);
  if (rc == FALSE) {
    ret = EXIT_FAILURE;
  }

  _endthreadex(ret);
  return 0;
}

using namespace roxyg::win32;

winrt::hresult WinEventHookController::start(
  DWORD event_min,
  DWORD event_max,
  WINEVENTPROC proc
) noexcept {
  WinEventHookThreadParams state{
    event_min, event_max,
    proc,
    ERROR_SUCCESS, false,
  };
  winrt::hresult hr = ThreadController::start(WinEventHookWorker, &state);
  if (FAILED(hr)) {
    return hr;
  }

  state.ready.wait(false, std::memory_order_acquire);

  DWORD const status = state.status.load(std::memory_order_acquire);
  if (status != ERROR_SUCCESS) {
    [[maybe_unused]] DWORD const stop_status = stop();
    return winrt::impl::hresult_from_win32(status);
  }

  return S_OK;
}

DWORD WinEventHookController::stop(DWORD timeout) noexcept {
  std::lock_guard<std::mutex> lock(mutex_);

  ThreadState const current_state{threadState()};
  DWORD status = validateThreadAccess(current_state);
  if (status != ERROR_SUCCESS) {
    return status;
  }

  BOOL rc = PostThreadMessageW(current_state.thread_id, WM_QUIT, 0, 0);
  DWORD post_status, wait_timeout;
  if (rc == FALSE) {
    post_status = WINRT_IMPL_GetLastError();
    wait_timeout = 0;  // force immediate return
  } else {
    post_status = ERROR_TIMEOUT;
    wait_timeout = timeout;
  }

  status = WaitForSingleObject(current_state.hthread, wait_timeout);
  switch (status) {
  case WAIT_TIMEOUT:
    return post_status;
  case WAIT_FAILED:
    return WINRT_IMPL_GetLastError();
  }

  return reapThread(current_state.hthread);
}
