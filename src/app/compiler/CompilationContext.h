#pragma once

#include "../monitor/MonitorStateCache.h"

namespace roxyg::compiler {

class CompilationContext final {
  CompilationContext(CompilationContext const&) = delete;
  CompilationContext& operator=(CompilationContext const&) = delete;

public:
  constexpr CompilationContext(monitor::StateCache& monitor_cache) noexcept
    : monitor_cache_(monitor_cache) {
  }

  HMONITOR resolveMonitorId(int id) const noexcept;

private:
  monitor::StateCache& monitor_cache_;
};

}
