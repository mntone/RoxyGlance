#pragma once
#include "WindowState.h"

namespace roxyg::window {

class StateCache final {
  using StatesType = boost::unordered_flat_map<HWND, State>;

  StateCache(StateCache const&) = delete;
  StateCache& operator=(StateCache const&) = delete;

public:
  constexpr StateCache() noexcept {
    states_.reserve(16);
  }

  [[nodiscard]] inline State* get(HWND hwnd) noexcept {
    StatesType::iterator it = states_.find(hwnd);
    return it != states_.end() ? &it->second : nullptr;
  }
  [[nodiscard]] inline State const* get(HWND hwnd) const noexcept {
    StatesType::const_iterator it = states_.find(hwnd);
    return it != states_.cend() ? &it->second : nullptr;
  }

  [[nodiscard]] State& getOrCreate(HWND hwnd) noexcept;

  void processWindowEvent(DWORD event, HWND hwnd) noexcept;

private:
  StatesType states_;
};

}
