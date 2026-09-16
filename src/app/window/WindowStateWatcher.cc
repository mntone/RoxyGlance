#include "pch.h"
#include "WindowStateWatcher.h"

#include "../win32/window.h"

using namespace roxyg::window;

StateWatcher::StateWatcher() noexcept
  : worker_() {
}

winrt::hresult StateWatcher::start() noexcept {
  return worker_.start(
    EVENT_SYSTEM_FOREGROUND,
    EVENT_OBJECT_NAMECHANGE,
    &StateWatcher::winEventProcStatic
  );
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
    State* const window_state = watcher.cache_.getOrCreate(hwnd);
    if (!window_state) {
      return;
    }
    watcher.notify(&IWindowStateListener::onForegroundEvent, *window_state);
  }

  if (object_id != OBJID_WINDOW || child_id != CHILDID_SELF) {
    return;
  }

  if (!win32::IsRootWindow(hwnd)) {
    return;
  }

  if (event == EVENT_OBJECT_SHOW) {
    State* const window_state = watcher.cache_.getOrCreate(hwnd);
    if (!window_state) {
      return;
    }
    watcher.notify(&IWindowStateListener::onShowEvent, *window_state);
  } else {
    watcher.cache_.processWindowEvent(event, hwnd);
  }
}
