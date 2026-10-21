#pragma once

#ifdef __INTELLISENSE__
#include <string>
#include "../../../app/utility/macro.h"
#endif

#include "../../../app/settings/Action.h"
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

ROXYG_ALWAYS_INLINE ::roxyg::settings::ActionType
toSettingsActionType(::winrt::Mntone::RoxyGlance::ViewModels::Settings::ActionType value) {
  using SAT = ::roxyg::settings::ActionType;
  using RAT = ::winrt::Mntone::RoxyGlance::ViewModels::Settings::ActionType;

  SAT ret;
  switch (value) {
  case RAT::AbsoluteMoveAndResize:
    ret = SAT::kAbsoluteMoveAndResize;
    break;
  case RAT::RelativeMoveAndResize:
    ret = SAT::kRelativeMoveAndResize;
    break;
  case RAT::Invalid:
    ret = SAT::kInvalid;
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw winrt::hresult_invalid_argument();
  }
  return ret;
}

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
