#pragma once
#include <winrt/Mntone.RoxyGlance.ViewModels.Settings.h>

#ifdef __INTELLISENSE__
#include <string>
#include "../../../app/utility/macro.h"
#endif

#include "../../../app/settings/Filter.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings {
using StringAndMatchType = ::std::pair<StringMatchType, hstring>;

inline constexpr size_t kFilterKeyCount = 3;
inline constexpr FilterKey kAllFilterKey[]{
  FilterKey::ProcessName,
  FilterKey::WindowClass,
  FilterKey::WindowTitle,
};
}

namespace roxyg::winui3::viewmodel::settings {

bool hasFilterForKey(
  ::roxyg::settings::Filter const& filter,
  ::winrt::Mntone::RoxyGlance::ViewModels::Settings::FilterKey key
);


// --- WinRT to Settings types

void setSettingsStringFilterValue(
  ::roxyg::settings::Filter& filter,
  ::winrt::Mntone::RoxyGlance::ViewModels::Settings::FilterKey key,
  ::winrt::Mntone::RoxyGlance::ViewModels::Settings::StringMatchType type,
  ::winrt::hstring str
);

ROXYG_ALWAYS_INLINE void clearSettingsStringFilterValue(
  ::roxyg::settings::Filter& filter,
  ::winrt::Mntone::RoxyGlance::ViewModels::Settings::FilterKey key
) {
  setSettingsStringFilterValue(
    filter,
    key,
    ::winrt::Mntone::RoxyGlance::ViewModels::Settings::StringMatchType::Contains,
    L""
  );
}

}
