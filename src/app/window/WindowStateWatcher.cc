#include "pch.h"
#include "WindowStateWatcher.h"

#include "../win32/window.h"

struct ThreadState final {
  std::atomic<bool> ready;
  std::atomic<DWORD> status;
};

static __forceinline void notifyBeginThread(ThreadState& state) noexcept {
  state.ready.store(true, std::memory_order_release);
  state.ready.notify_one();
}

using namespace roxyg::window;

unsigned int __stdcall StateWatcher::winEventThreadWorker(void* params) noexcept {
  ThreadState& state = *static_cast<ThreadState*>(params);
  HWINEVENTHOOK hWinEventHook = SetWinEventHook(
    EVENT_SYSTEM_FOREGROUND, EVENT_OBJECT_NAMECHANGE,
    nullptr,
    &StateWatcher::winEventProcStatic,
    0,
    0,
    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
  if (!hWinEventHook) {
    state.status.store(WINRT_IMPL_GetLastError(), std::memory_order_relaxed);
    notifyBeginThread(state);
    _endthreadex(EXIT_FAILURE);
    return 0;
  }

  MSG msg;
  BOOL rc = PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
  notifyBeginThread(state);

  DWORD ret = EXIT_SUCCESS;
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

StateWatcher::StateWatcher() noexcept
  : worker_() {
}

winrt::hresult StateWatcher::start() noexcept {
  ThreadState state{false, ERROR_SUCCESS};
  winrt::hresult hr = worker_.start(StateWatcher::winEventThreadWorker, &state);
  if (FAILED(hr)) {
    return hr;
  }

  state.ready.wait(false, std::memory_order_acquire);

  DWORD const status = state.status.load(std::memory_order_acquire);
  if (status != ERROR_SUCCESS) {
    [[maybe_unused]] DWORD const stop_status = worker_.stop();
    return winrt::impl::hresult_from_win32(status);
  }

  return S_OK;
}

winrt::hresult StateWatcher::stop() noexcept {
  return winrt::impl::hresult_from_win32(worker_.stop());
}

void CALLBACK StateWatcher::winEventProcStatic(
  [[maybe_unused]] HWINEVENTHOOK hWinEventHook,
  DWORD event,
  HWND hwnd,
  LONG object_id,
  LONG child_id,
  [[maybe_unused]] DWORD idEventThread,
  [[maybe_unused]] DWORD dwmsEventTime
) noexcept {
  StateWatcher& watcher = StateWatcher::instance();
  if (event == EVENT_SYSTEM_FOREGROUND) {
    State& windowState = watcher.cache_.getOrCreate(hwnd);
    watcher.notify(&IWindowStateListener::onForegroundEvent, windowState);
  }

  if (object_id != OBJID_WINDOW || child_id != CHILDID_SELF) {
    return;
  }

  if (!win32::IsRootWindow(hwnd)) {
    return;
  }

  if (event == EVENT_OBJECT_SHOW) {
    State& windowState = watcher.cache_.getOrCreate(hwnd);
    watcher.notify(&IWindowStateListener::onShowEvent, windowState);
  } else {
    watcher.cache_.processWindowEvent(event, hwnd);
  }
}
