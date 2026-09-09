#include "pch.h"
#include "MonitorState.h"

using namespace roxyg::monitor;

void State::updateArea() {
  MONITORINFOEXW info{ sizeof(MONITORINFOEXW) };
  BOOL rc = GetMonitorInfoW(hmonitor_, &info);
  if (!rc) {
    winrt::throw_hresult(E_FAIL);
  }

  display_area_ = static_cast<numeric::float4>(numeric::long4::make(
    info.rcMonitor.left,
    info.rcMonitor.top,
    info.rcMonitor.right - info.rcMonitor.left,
    info.rcMonitor.bottom - info.rcMonitor.top
  ));
  work_area_ = static_cast<numeric::float4>(numeric::long4::make(
    info.rcWork.left,
    info.rcWork.top,
    info.rcWork.right - info.rcWork.left,
    info.rcWork.bottom - info.rcWork.top
  ));
  device_name_ = info.szDevice;
  area_dirty_ = false;
}
