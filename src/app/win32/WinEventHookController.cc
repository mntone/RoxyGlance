#include "pch.h"
#include "WinEventHookController.h"
#include "ThreadContext.h"

#include "../win32/hresult.h"

namespace {

inline constexpr std::wstring_view kPostWinEventHookThreadQuitMessageFailedQuota
  = L"Failed to post a quit message to the WinEvent hook thread. Not enough quota is available.";
inline constexpr std::wstring_view kPostWinEventHookThreadQuitMessageFailed
  = L"Failed to post a quit message to the WinEvent hook thread.";
inline constexpr std::wstring_view kWaitForWinEventHookThreadExitFailed
  = L"Failed to wait for the WinEvent hook thread to exit.";
inline constexpr std::wstring_view kWaitForWinEventHookThreadExitTimeout
  = L"The wait for the WinEvent hook thread to exit timed out.";

}

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
  winrt::hresult hr = ThreadController::start(WinEventHookWorker, &state);
  if (FAILED(hr)) {
    return hr;
  }

  winrt::hresult const hresult = state.wait_and_load();
  if (hresult != S_OK) {
    [[maybe_unused]] DWORD const stop_status = stop();
  }

  return hresult;
}

winrt::hresult WinEventHookController::stop(DWORD timeout) noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
  if (state_ != State::kRunning) {
    return hresult::kErrorInvalidOperation;
  }

  ThreadInfo const current_info{threadInfo()};
  DWORD status = validateThreadAccess(current_info);
  if (status != ERROR_SUCCESS) {
    return hresult::HResultFromWin32(status);
  }

  // Record the stopping state before requesting thread exit.
  state_ = State::kStopping;

  BOOL rc = PostThreadMessageW(current_info.thread_id, WM_QUIT, 0, 0);
  DWORD delay;
  if (rc == FALSE) {
    DWORD const lasterr = WINRT_IMPL_GetLastError();
    if (lasterr == ERROR_NOT_ENOUGH_QUOTA) {
      delay = timeout;
      logger_.notice(winrt::hstring{kPostWinEventHookThreadQuitMessageFailedQuota}, hresult::kErrorNotEnoughQuota);
    } else {
      delay = 0;  // force immediate return
      logger_.error(winrt::hstring{kPostWinEventHookThreadQuitMessageFailed}, hresult::HResultFromWin32(lasterr));
    }
  } else {
    delay = timeout;
  }

  status = WaitForSingleObject(current_info.hthread, delay);
  if (status != WAIT_OBJECT_0) {
    if (status == WAIT_TIMEOUT) {
      logger_.error(winrt::hstring{kWaitForWinEventHookThreadExitTimeout}, hresult::kErrorTimeout);
    } else {
      winrt::hresult hr = hresult::LastErrorAsHResult();
      logger_.error(winrt::hstring{kWaitForWinEventHookThreadExitFailed}, hr);
    }
    return forceExitThread(current_info.hthread);
  }

  return reapThread(current_info.hthread);
}
