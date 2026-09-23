#pragma once

namespace roxyg::win32 {

ROXYG_ALWAYS_INLINE HMONITOR GetPrimaryHMonitor() noexcept {
#pragma warning(push)
#pragma warning(disable:6387)
  HMONITOR const hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
#pragma warning(pop)
  return hMonitor;
}

ROXYG_ALWAYS_INLINE int GetMonitorCount() noexcept {
  int const monitorCount = GetSystemMetrics(SM_CMONITORS);
  return monitorCount;
}

}
