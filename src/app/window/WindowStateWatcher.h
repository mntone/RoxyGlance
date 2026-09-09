#pragma once
#include "IWindowStateListener.h"
#include "WindowStateCache.h"

#include "../utility/ListenerHost.h"

namespace roxyg::window {

class StateWatcher final
  : public utility::ListenerHost<IWindowStateListener> {
  StateWatcher() noexcept = default;
  StateWatcher(StateWatcher const&) = delete;
  StateWatcher& operator=(StateWatcher const&) = delete;

public:
  [[nodiscard]] winrt::hresult start() noexcept;
  [[nodiscard]] winrt::hresult stop() noexcept;

  [[nodiscard]] static StateWatcher& instance() noexcept {
    static StateWatcher instance;
    return instance;
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
  HWINEVENTHOOK hWinEventHook_{nullptr};
  StateCache cache_;
};

}
