#include "pch.h"
#include "monitor.h"

#include "hresult.h"

#include <boost/container/small_vector.hpp>

namespace roxyg::win32 {

// QDC_VIRTUAL_MODE_AWARE requires Windows 10 1607 or later.
inline constexpr UINT kDisplayConfigFlags = QDC_ONLY_ACTIVE_PATHS | QDC_VIRTUAL_MODE_AWARE;
inline constexpr int kDisplayConfigMaxRetries = 3;

winrt::hresult GetMonitorFriendlyNames(MonitorFriendlyNameContainer& friendly_names) noexcept try {
  LONG rc = ERROR_SUCCESS;

  int count = kDisplayConfigMaxRetries;
  boost::container::small_vector<DISPLAYCONFIG_PATH_INFO, 6> paths;
  boost::container::small_vector<DISPLAYCONFIG_MODE_INFO, 6> modes;
  do {
    UINT32 path_count, mode_count;
    rc = GetDisplayConfigBufferSizes(kDisplayConfigFlags, &path_count, &mode_count);
    if (rc != ERROR_SUCCESS) {
      return hresult::HResultFromWin32(rc);
    }
    paths.resize(path_count);
    modes.resize(mode_count);

    rc = QueryDisplayConfig(kDisplayConfigFlags, &path_count, paths.data(), &mode_count, modes.data(), nullptr);
    if (rc == ERROR_SUCCESS) {
      paths.resize(path_count);
      modes.resize(mode_count);
    } else if (rc != ERROR_INSUFFICIENT_BUFFER) {
      return hresult::HResultFromWin32(rc);
    }
  } while (rc == ERROR_INSUFFICIENT_BUFFER && --count > 0);
  if (rc != ERROR_SUCCESS) {
    return hresult::HResultFromWin32(rc);
  }

  winrt::hresult hr = S_OK;
  for (DISPLAYCONFIG_PATH_INFO const& path : paths) {
    DISPLAYCONFIG_SOURCE_DEVICE_NAME source_name{
      .header = {
        .type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME,
        .size = sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME),
        .adapterId = path.sourceInfo.adapterId,
        .id = path.sourceInfo.id,
      },
    };
    rc = DisplayConfigGetDeviceInfo(&source_name.header);
    if (rc != ERROR_SUCCESS) {
      hr = hresult::HResultFromWin32(rc);
      continue;
    }

    DISPLAYCONFIG_TARGET_DEVICE_NAME target_name{
      .header = {
        .type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME,
        .size = sizeof(DISPLAYCONFIG_TARGET_DEVICE_NAME),
        .adapterId = path.targetInfo.adapterId,
        .id = path.targetInfo.id,
      },
    };
    rc = DisplayConfigGetDeviceInfo(&target_name.header);
    if (rc != ERROR_SUCCESS) {
      hr = hresult::HResultFromWin32(rc);
      continue;
    }
    if (!target_name.flags.friendlyNameFromEdid) {
      continue;
    }

    int const device_id{GetMonitorDeviceId(source_name.viewGdiDeviceName)};
    friendly_names.try_emplace(device_id, target_name.monitorFriendlyDeviceName);
  }
  return hr;
} catch (std::bad_alloc const&) {
  return E_OUTOFMEMORY;
} catch (std::length_error const&) {
  return E_INVALIDARG;
}

}
