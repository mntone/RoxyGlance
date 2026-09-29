#include "pch.h"
#include "MonitorState.h"

#include "../win32/hresult.h"

using namespace roxyg::monitor;

State::State(HMONITOR handle, numeric::float4 display_area) noexcept
  : display_area_(display_area)
  , work_area_(numeric::float4::make(0.f, 0.f, 0.f, 0.f))
  , handle_(handle)
  , id_(0) {
}

State::State(HMONITOR handle) noexcept
  : display_area_(numeric::float4::make(0.f, 0.f, 0.f, 0.f))
  , work_area_(numeric::float4::make(0.f, 0.f, 0.f, 0.f))
  , handle_(handle)
  , id_(0) {
}

winrt::hresult State::initialize() noexcept {
  MONITORINFOEXW info{sizeof(MONITORINFOEXW)};
  BOOL const rc = GetMonitorInfoW(handle_, &info);
  if (rc == FALSE) {
    return win32::hresult::kErrorInvalidMonitorHandle;
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
  id_ = _wtoi(&info.szDevice[11]);
  return S_OK;
}
