#pragma once
#include "ViewModels/Settings/RulesViewModel.g.h"

#include "RuleViewModel.h"

#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct RulesViewModel
  : RulesViewModelT<RulesViewModel>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<RulesViewModel> {
  RulesViewModel(roxyg::settings::Rules& rules);

  inline Windows::Foundation::Collections::IObservableVector<ViewModels::Settings::RuleViewModel> Items() const noexcept {
    return Items_;
  }

  DEFINE_REFPROP(ViewModels::Settings::RuleViewModel, SelectedItem);

  void save();

  static void initialize();

private:
  Windows::Foundation::Collections::IObservableVector<ViewModels::Settings::RuleViewModel> Items_;
  ViewModels::Settings::RuleViewModel SelectedItem_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs SelectedItemPropertyChangedEventArgs_;
};

}
