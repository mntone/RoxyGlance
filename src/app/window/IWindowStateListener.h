#pragma once

namespace roxyg::window {

struct State;
struct IWindowStateListener {
  virtual void onForegroundEvent(State& windowState) noexcept = 0;
  virtual void onShowEvent(State& windowState) noexcept = 0;
};

}
