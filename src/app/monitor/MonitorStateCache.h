#pragma once
#include "MonitorState.h"

#include "../utility/ValueCache.h"

namespace roxyg::monitor {

/// <summary>
/// Caches monitor states by monitor handle.
/// </summary>
/// <remarks>
/// Before invalidating or rebuilding the cache, ensure that all workers
/// accessing monitor states have completed.
/// </remarks>
class StateCache final
  : public utility::ValueCache<HMONITOR, State> {
public:
  [[nodiscard]] winrt::hresult initialize() noexcept;

private:
  static BOOL CALLBACK EnumDisplayMonitorsCallback(HMONITOR hmonitor, HDC, LPRECT lprcMonitor, LPARAM data) noexcept;
};

}
