#include "pch.h"
#include "RulesViewModel.h"
#if __has_include("ViewModels/Settings/RulesViewModel.g.cpp")
#include "ViewModels/Settings/RulesViewModel.g.cpp"
#endif

#include "RuleViewModel.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;

using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Data;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs RulesViewModel::SelectedItemPropertyChangedEventArgs_{nullptr};

void RulesViewModel::initialize() {
  SelectedItemPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"SelectedItem"};
}

static ROXYG_ALWAYS_INLINE IObservableVector<winrt::RuleViewModel> makeRuleViewModels(
  roxyg::settings::Rules& rules,
  RuleViewModel::CallbackFunction callback,
  IInspectable const& context
) {
  std::vector<winrt::RuleViewModel> winrt_rules;
  winrt_rules.reserve(rules.size());
  for (roxyg::settings::Rule& rule : rules) {
    winrt_rules.push_back(make<RuleViewModel>(rule, callback, context));
  }
  return single_threaded_observable_vector(std::move(winrt_rules));
}

RulesViewModel::RulesViewModel(roxyg::settings::Rules& rules)
  : Items_(makeRuleViewModels(rules, &RulesViewModel::onRuleHasPendingChangesUpdated, *this))
  , SelectedItem_(Items_.Size() > 0 ? Items_.GetAt(0) : nullptr) {
}

void RulesViewModel::SelectedItem(winrt::RuleViewModel const& value) {
  setProperty(SelectedItem_, value, SelectedItemPropertyChangedEventArgs_);
}

bool RulesViewModel::HasPendingChanges() const noexcept {
  for (winrt::RuleViewModel const& rule : Items_) {
    if (rule.HasPendingChanges()) {
      return true;
    }
  }
  return false;
}

bool RulesViewModel::applyChanges() {
  bool changed = false;
  for (winrt::RuleViewModel const& vm : Items_) {
    if (get_self<RuleViewModel>(vm)->applyChanges()) {
      changed = true;
    }
  }
  return changed;
}

void RulesViewModel::onRuleHasPendingChangesUpdated(IInspectable const& context) {
  com_ptr<RulesViewModel> self{context.as<RulesViewModel>()};
  self->notifyPropertyChanged(HasPendingChangesPropertyChangedEventArgs_);
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
