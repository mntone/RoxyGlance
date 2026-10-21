#pragma once
#include "ViewModels/Settings/RulesViewModel.g.h"
#include "SettingsViewModelProperties.h"

#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct RulesViewModel
  : RulesViewModelT<RulesViewModel, non_agile>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<RulesViewModel>
  , SettingsViewModelProperties {
  RulesViewModel(roxyg::settings::Rules& rules);

  inline Windows::Foundation::Collections::IObservableVector<ViewModels::Settings::RuleViewModel> Items() const noexcept {
    return Items_;
  }

  DEFINE_REFPROP(ViewModels::Settings::RuleViewModel, SelectedItem);

  bool HasPendingChanges() const noexcept;
  bool applyChanges();

  static void initialize();

private:
  static void onRuleHasPendingChangesUpdated(Windows::Foundation::IInspectable const& context);

private:
  Windows::Foundation::Collections::IObservableVector<ViewModels::Settings::RuleViewModel> Items_;
  ViewModels::Settings::RuleViewModel SelectedItem_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs SelectedItemPropertyChangedEventArgs_;
};

}
