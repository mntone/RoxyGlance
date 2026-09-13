#include "pch.h"
#include "OperationContext.h"

using namespace roxyg;

winrt::hresult OperationContext::initialize() noexcept {
  return monitor_state_cache_.initialize();
}
