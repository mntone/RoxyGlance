#pragma once
#include "MonitorState.h"

namespace roxyg::monitor {

class StateCache final {
  using StatesType = boost::unordered_flat_map<HMONITOR, std::shared_ptr<State>>;

  StateCache(StateCache const&) = delete;
  StateCache& operator=(StateCache const&) = delete;

public:
  constexpr StateCache() noexcept {
    states_.reserve(4);
  }

  [[nodiscard]] inline std::shared_ptr<State> get(HMONITOR hMonitor) noexcept {
    StatesType::iterator it = states_.find(hMonitor);
    return it != states_.end() ? it->second : nullptr;
  }

  [[nodiscard]] inline std::shared_ptr<State> getOrCreate(HMONITOR hMonitor) noexcept {
    auto [it, emplaced] = states_.try_emplace(hMonitor);
    if (emplaced) {
      std::shared_ptr<State> newState{ std::make_shared<State>() };
      newState->initialize(hMonitor);
      it->second = std::move(newState);
    }
    return it->second;
  }

  [[nodiscard]] inline void remove(HMONITOR hMonitor) noexcept {
    states_.erase(hMonitor);
  }

private:
  StatesType states_;
};

}
