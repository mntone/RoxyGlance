#include "pch.h"
#include "RulesViewModel.h"
#if __has_include("ViewModels/Settings/RulesViewModel.g.cpp")
#include "ViewModels/Settings/RulesViewModel.g.cpp"
#endif

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

static ROXYG_ALWAYS_INLINE IObservableVector<winrt::RuleViewModel> makeRuleViewModels(roxyg::settings::Rules& rules) {
  std::vector<winrt::RuleViewModel> winrt_rules;
  winrt_rules.reserve(rules.size());
  for (roxyg::settings::Rule& rule : rules) {
    winrt_rules.push_back(make<RuleViewModel>(rule));
  }
  return single_threaded_observable_vector(std::move(winrt_rules));
}

RulesViewModel::RulesViewModel(roxyg::settings::Rules& rules)
  : Items_(makeRuleViewModels(rules))
  , SelectedItem_(Items_.Size() > 0 ? Items_.GetAt(0) : nullptr) {
}

void RulesViewModel::SelectedItem(winrt::RuleViewModel const& value) {
  setProperty(SelectedItem_, value, SelectedItemPropertyChangedEventArgs_);
}

void RulesViewModel::save() {
  for (winrt::RuleViewModel const& vm : Items_) {
    get_self<RuleViewModel>(vm)->save();
  }
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
