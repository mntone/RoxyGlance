#include "pch.h"
#include "CompilationContext.h"

#include "../win32/monitor.h"

using namespace roxyg::compiler;

HMONITOR CompilationContext::resolveMonitorId(int id) const noexcept {
  if (id == 0) {
    return win32::GetPrimaryHMonitor();
  }

  monitor::State const* state{monitor_cache_.findById(id)};
  if (!state) {
    return nullptr;
  }

  return state->handle();
}
