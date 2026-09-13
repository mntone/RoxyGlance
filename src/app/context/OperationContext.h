#pragma once

#include "../monitor/MonitorStateCache.h"

namespace roxyg {

class OperationContext final {
  OperationContext(OperationContext const&) = delete;
  OperationContext& operator=(OperationContext const&) = delete;

public:
  constexpr OperationContext() noexcept = default;

  [[nodiscard]] winrt::hresult initialize() noexcept;

  constexpr monitor::StateCache& monitor() noexcept {
    return monitor_state_cache_;
  }

private:
  monitor::StateCache monitor_state_cache_;
};

}
