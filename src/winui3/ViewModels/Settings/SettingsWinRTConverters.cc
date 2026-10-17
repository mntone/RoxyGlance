#include "pch.h"
#include "SettingsWinRTConverters.h"

namespace winrt {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

using namespace roxyg::settings;

namespace roxyg::winui3::viewmodel::settings {

static ROXYG_ALWAYS_INLINE bool hasFilterValue(StringAndMatchType const& value) {
  return value.first != StringMatchType::kContains || !value.second.empty();
}

bool hasFilterForKey(Filter const& filter, winrt::FilterKey key) {
  StringAndMatchType const* value;
  switch (key) {
  case winrt::FilterKey::ProcessName:
    value = &filter.processImageName();
    break;
  case winrt::FilterKey::WindowClass:
    value = &filter.windowClass();
    break;
  case winrt::FilterKey::WindowTitle:
    value = &filter.windowTitle();
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw winrt::hresult_invalid_argument();
  }

  return hasFilterValue(*value);
}


static ROXYG_ALWAYS_INLINE StringMatchType
toSettingsStringMatchType(winrt::StringMatchType value) {
  StringMatchType ret;
  switch (value) {
  case winrt::StringMatchType::Contains:
    ret = StringMatchType::kContains;
    break;
  case winrt::StringMatchType::Equals:
    ret = StringMatchType::kEquals;
    break;
  case winrt::StringMatchType::StartsWith:
    ret = StringMatchType::kStartsWith;
    break;
  case winrt::StringMatchType::EndsWith:
    ret = StringMatchType::kEndsWith;
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw winrt::hresult_invalid_argument();
  }
  return ret;
}

static ROXYG_ALWAYS_INLINE StringAndMatchType
toSettingsStringAndMatch(winrt::StringMatchType type, winrt::hstring str) {
  return {toSettingsStringMatchType(type), std::wstring{str}};
}

void setSettingsStringFilterValue(
  Filter& filter,
  winrt::FilterKey key,
  winrt::StringMatchType type,
  winrt::hstring str
) {
  StringAndMatchType const value{toSettingsStringAndMatch(type, str)};
  switch (key) {
  case winrt::FilterKey::ProcessName:
    filter.setProcessImageName(value);
    break;
  case winrt::FilterKey::WindowClass:
    filter.setWindowClass(value);
    break;
  case winrt::FilterKey::WindowTitle:
    filter.setWindowTitle(value);
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw winrt::hresult_invalid_argument();
  }
}

}  // namespace roxyg::winui3::viewmodel::settings
