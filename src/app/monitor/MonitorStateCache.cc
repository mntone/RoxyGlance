#include "pch.h"
#include "MonitorStateCache.h"

#include "../win32/monitor.h"

struct MonitorCallbackData final {
  roxyg::monitor::StateCache::StatesType states;
  winrt::hresult hresult;
};

using namespace roxyg::monitor;

BOOL __stdcall StateCache::EnumDisplayMonitorsCallback(
  HMONITOR hmonitor,
  HDC,
  LPRECT lprcMonitor,
  LPARAM d
) noexcept {
  using float4 = roxyg::numeric::float4;
  using long4 = roxyg::numeric::long4;

  long4 const display_area_long = long4::make(
    lprcMonitor->left,
    lprcMonitor->top,
    lprcMonitor->right - lprcMonitor->left,
    lprcMonitor->bottom - lprcMonitor->top
  );
  float4 const display_area = static_cast<float4>(display_area_long);
  MonitorCallbackData& data = *reinterpret_cast<MonitorCallbackData*>(d);

  // Handle display mirroring edge cases: Certain graphics drivers create a distinct
  // HMONITOR for each physical display even when sharing identical desktop coordinates (RECT).
  // Because EnumDisplayMonitors guarantees that the primary display is enumerated first,
  // ignoring duplicate coordinates effectively filters out the secondary mirrored monitor.
  for (auto& [_, state] : data.states) {
    if (state.display_area_ == display_area) {
      return TRUE;
    }
  }

  try {
    data.states.try_emplace(hmonitor, hmonitor, display_area);
  } catch (std::bad_alloc const&) {
    data.hresult = E_OUTOFMEMORY;
    return FALSE;
  }
  return TRUE;
}

winrt::hresult StateCache::initialize() noexcept {
  int const monitor_count = win32::GetMonitorCount();
  if (monitor_count <= 0) {
    states_.clear();
    return S_OK;
  }

  MonitorCallbackData data{{}, E_FAIL};
  try {
    data.states.reserve(monitor_count);
  } catch (std::bad_alloc const&) {
    return E_OUTOFMEMORY;
  }

  BOOL rc = EnumDisplayMonitors(
    nullptr,
    nullptr,
    EnumDisplayMonitorsCallback,
    reinterpret_cast<LPARAM>(&data)
  );
  if (rc == FALSE) {
    return data.hresult;
  }

  states_ = std::move(data.states);
  return S_OK;
}
