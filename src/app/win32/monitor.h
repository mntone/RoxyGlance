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

/// <summary>
/// Extracts the display number from a GDI device name such as "\\.\DISPLAY1".
/// </summary>
/// <param name="device_name">A null-terminated name that starts with "\\.\DISPLAY".</param>
/// <returns>The display number, or 0 if no number follows the prefix.</returns>
ROXYG_ALWAYS_INLINE int GetMonitorDeviceId(wchar_t const* device_name) noexcept {
#ifdef _DEBUG
  assert(wcsncmp(device_name, L"\\\\.\\DISPLAY", 11) == 0);
#endif
  return _wtoi(&device_name[11]);
}

// EDID monitor names are at most 13 characters; 14 makes the type exactly 32 bytes.
using MonitorFriendlyName = boost::static_wstring<14>;
using MonitorFriendlyNameContainer = boost::unordered::unordered_flat_map<int, MonitorFriendlyName>;

/// <summary>
/// Collects EDID-based friendly names of active monitors, keyed by display number.
/// </summary>
/// <param name="friendly_names">Receives the names; monitors without an EDID name are skipped.</param>
/// <returns>S_OK on success, or the last error if some monitors failed; successfully retrieved names are kept in either case.</returns>
winrt::hresult GetMonitorFriendlyNames(MonitorFriendlyNameContainer& friendly_names) noexcept;

}
