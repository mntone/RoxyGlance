#pragma once
#include "IWindowStateListener.h"
#include "WindowStateCache.h"

#include "../utility/ListenerHost.h"
#include "../win32/WinEventHookController.h"

namespace roxyg::window {

class StateWatcher final
  : public utility::ListenerHost<IWindowStateListener> {
  StateWatcher() noexcept;
  StateWatcher(StateWatcher const&) = delete;
  StateWatcher& operator=(StateWatcher const&) = delete;

public:
  [[nodiscard]] winrt::hresult start() noexcept;
  [[nodiscard]] winrt::hresult stop() noexcept;

  [[nodiscard]] static StateWatcher& instance() noexcept {
    static StateWatcher instance;
    return instance;
  }

  constexpr void setLogger(logging::Logger* logger) noexcept {
    worker_.setLogger(logger);
  }

private:
  static void CALLBACK winEventProcStatic(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG object_id,
    LONG child_id,
    DWORD idEventThread,
    DWORD dwmsEventTime
  ) noexcept;

private:
  win32::WinEventHookController worker_;
  StateCache cache_;
};

}
