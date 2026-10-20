#include "pch.h"
#include "StringFilterViewModel.h"
#if __has_include("ViewModels/Settings/StringFilterViewModel.g.cpp")
#include "ViewModels/Settings/StringFilterViewModel.g.cpp"
#endif

#include "SettingsWinRTConverters.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Data;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

using namespace roxyg::winui3::viewmodel::settings;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs StringFilterViewModel::ValueMatchPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs StringFilterViewModel::ValuePropertyChangedEventArgs_{nullptr};

void StringFilterViewModel::initialize() {
  ValueMatchPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"ValueMatchType"};
  ValuePropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Value"};
}


// --- Settings to WinRT types

static ROXYG_ALWAYS_INLINE StringMatchType
toWinRTStringCompareType(roxyg::settings::StringMatchType value) {
  using SMT = roxyg::settings::StringMatchType;

  StringMatchType ret;
  switch (value) {
  case SMT::kContains:
    ret = StringMatchType::Contains;
    break;
  case SMT::kEquals:
    ret = StringMatchType::Equals;
    break;
  case SMT::kStartsWith:
    ret = StringMatchType::StartsWith;
    break;
  case SMT::kEndsWith:
    ret = StringMatchType::EndsWith;
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw hresult_invalid_argument();
  }
  return ret;
}

static ROXYG_ALWAYS_INLINE StringAndMatchType
toWinRTStringAndCompare(roxyg::settings::StringAndMatchType pair) {
  return {toWinRTStringCompareType(pair.first), hstring{pair.second}};
}

static ROXYG_ALWAYS_INLINE StringAndMatchType
toWinRTStringFilterValue(roxyg::settings::Filter const& filter, FilterKey key) {
  switch (key) {
  case FilterKey::ProcessName:
    return toWinRTStringAndCompare(filter.processImageName());
  case FilterKey::WindowClass:
    return toWinRTStringAndCompare(filter.windowClass());
  case FilterKey::WindowTitle:
    return toWinRTStringAndCompare(filter.windowTitle());
  }
#ifdef _DEBUG
  __debugbreak();
#endif
  throw hresult_invalid_argument();
}


// --- Implementation

StringFilterViewModel::StringFilterViewModel(
  roxyg::settings::Filter& filter,
  FilterKey key,
  CallbackFunction callback,
  IInspectable const& context,
  bool new_elem
)
  : StaticCommandHolder(&StringFilterViewModel::onDelete)
  , CallbackHolder(callback, context)
  , filter_(filter)
  , Key_(key)
  , dirty_(new_elem) {
  if (new_elem) {
    ValueMatchType_ = StringMatchType::Contains;
    Value_ = L"";
  } else {
    auto value_pair = toWinRTStringFilterValue(filter, key);
    ValueMatchType_ = value_pair.first;
    Value_ = value_pair.second;
  }
}

void StringFilterViewModel::ValueMatchType(StringMatchType compare) {
  if (setProperty(ValueMatchType_, compare, ValueMatchPropertyChangedEventArgs_)) {
    dirty_ = true;
  }
}

void StringFilterViewModel::Value(hstring value) {
  if (setProperty(Value_, value, ValuePropertyChangedEventArgs_)) {
    dirty_ = true;
  }
}

void StringFilterViewModel::save() {
  if (!dirty_) {
    return;
  }

  setSettingsStringFilterValue(filter_, Key_, ValueMatchType_, Value_);
  dirty_ = false;
}

void StringFilterViewModel::onDelete(StringFilterViewModel& self, IInspectable const&) {
  self.invokeCallback(self.Key_);
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
