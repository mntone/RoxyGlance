#pragma once

namespace roxyg::win32 {

__forceinline HMONITOR GetPrimaryHMonitor() noexcept {
#pragma warning(push)
#pragma warning(disable:6387)
  HMONITOR const hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
#pragma warning(pop)
  return hMonitor;
}

}
