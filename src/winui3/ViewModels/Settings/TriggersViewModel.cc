#include "pch.h"
#include "TriggersViewModel.h"
#if __has_include("ViewModels/Settings/TriggersViewModel.g.cpp")
#include "ViewModels/Settings/TriggersViewModel.g.cpp"
#endif

#include <magic_enum/magic_enum_flags.hpp>

namespace winrt {
using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Data;
}

using namespace magic_enum;
using namespace roxyg::settings;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs TriggersViewModel::ApplicationStartedPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs TriggersViewModel::WindowActivatedPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs TriggersViewModel::WindowShownPropertyChangedEventArgs_{nullptr};

void TriggersViewModel::initialize() {
  ApplicationStartedPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"ApplicationStarted"};
  WindowActivatedPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"WindowActivated"};
  WindowShownPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"WindowShown"};
}

TriggersViewModel::TriggersViewModel(Rule& rule)
  : rule_(rule)
  , dirty_(false)
  , ApplicationStarted_(enum_flags_test(rule.triggerFlags(), TriggerFlags::kApplicationStart))
  , WindowActivated_(enum_flags_test(rule.triggerFlags(), TriggerFlags::kWindowFocus))
  , WindowShown_(enum_flags_test(rule.triggerFlags(), TriggerFlags::kWindowShow)) {
}

void TriggersViewModel::ApplicationStarted(bool flag) {
  if (setProperty(ApplicationStarted_, flag, ApplicationStartedPropertyChangedEventArgs_)) {
    dirty_ = true;
  }
}

void TriggersViewModel::WindowActivated(bool flag) {
  if (setProperty(WindowActivated_, flag, WindowActivatedPropertyChangedEventArgs_)) {
    dirty_ = true;
  }
}

void TriggersViewModel::WindowShown(bool flag) {
  if (setProperty(WindowShown_, flag, WindowShownPropertyChangedEventArgs_)) {
    dirty_ = true;
  }
}

void TriggersViewModel::save() {
  if (!dirty_) {
    return;
  }

  underlying_type_t<TriggerFlags> raw_trigger{enum_underlying(TriggerFlags::kNone)};
  if (ApplicationStarted_) {
    raw_trigger |= enum_underlying(TriggerFlags::kApplicationStart);
  }
  if (WindowActivated_) {
    raw_trigger |= enum_underlying(TriggerFlags::kWindowFocus);
  }
  if (WindowShown_) {
    raw_trigger |= enum_underlying(TriggerFlags::kWindowShow);
  }

  TriggerFlags const triggers{enum_flags_cast<TriggerFlags>(raw_trigger).value_or(TriggerFlags::kNone)};
  rule_.setTriggerFlags(triggers);

  dirty_ = false;
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
