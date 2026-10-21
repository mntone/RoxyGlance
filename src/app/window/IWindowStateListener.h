#pragma once

namespace roxyg::window {

struct State;
struct ROXYG_NOVTABLE IWindowStateListener {
  virtual void onForegroundEvent(State& windowState) noexcept = 0;
  virtual void onShowEvent(State& windowState) noexcept = 0;

protected:
  ~IWindowStateListener() = default;
};

}
