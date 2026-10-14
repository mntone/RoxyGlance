#include "pch.h"
#include "MonitorStateCache.h"

#include "../win32/monitor.h"

namespace {
inline constexpr std::wstring_view kFriendlyNameRetrieveFailed
  = L"Failed to retrieve monitor friendly names.";
}

struct MonitorCallbackData final {
  roxyg::win32::MonitorFriendlyNameContainer friendly_names;
  roxyg::monitor::StateCache::ContainerType states;
  winrt::hresult hresult;
};

using namespace roxyg::monitor;
using namespace roxyg::win32;

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
    auto it = data.states.try_emplace(hmonitor, hmonitor, display_area);
    ROXYG_UNCHECKED_ASSERT(it.second);

    State& state{it.first->second};
    winrt::hresult const hr = state.initialize();
    if (FAILED(hr)) {
      data.hresult = hr;
      return FALSE;
    }

    MonitorFriendlyNameContainer::const_iterator name_it{data.friendly_names.find(state.id())};
    if (name_it != data.friendly_names.cend()) {
      state.setFriendlyName(name_it->second);
    }
  } catch (std::bad_alloc const&) {
    data.hresult = E_OUTOFMEMORY;
    return FALSE;
  }
  return TRUE;
}

winrt::hresult StateCache::initialize() noexcept {
  int const monitor_count = GetMonitorCount();
  if (monitor_count <= 0) {
    storage_.clear();
    return S_OK;
  }

  MonitorCallbackData data{{}, {}, E_FAIL};
  try {
    data.states.reserve(monitor_count);
  } catch (std::bad_alloc const&) {
    return E_OUTOFMEMORY;
  }

  winrt::hresult const hr = GetMonitorFriendlyNames(data.friendly_names);
  if (FAILED(hr)) {
    log_.warn(winrt::hstring{kFriendlyNameRetrieveFailed}, hr);
  }

  BOOL const rc = EnumDisplayMonitors(
    nullptr,
    nullptr,
    EnumDisplayMonitorsCallback,
    reinterpret_cast<LPARAM>(&data)
  );
  if (rc == FALSE) {
    return data.hresult;
  }

  storage_ = std::move(data.states);
  return S_OK;
}

State const* StateCache::findById(int id) const noexcept {
  for (auto const& [_, val] : storage_) {
    if (val.id() == id) {
      return &val;
    }
  }
  return nullptr;
}
