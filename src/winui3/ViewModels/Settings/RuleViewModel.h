#pragma once
#include "ViewModels/Settings/RuleViewModel.g.h"

#ifdef __INTELLISENSE__
#include <boost/container/small_vector.hpp>
#endif

#include <winrt/Mntone.RoxyGlance.ViewModels.Settings.h>
#include "StringFilterViewModel.h"

#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../Holders/StaticCommandHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct RuleViewModel
  : RuleViewModelT<RuleViewModel, Microsoft::UI::Xaml::Input::ICommand>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<RuleViewModel>
  , roxyg::winui3::viewmodel::StaticCommandHolder<RuleViewModel> {
  RuleViewModel(roxyg::settings::Rule& rule);

  inline Microsoft::UI::Xaml::Input::ICommand AddFilterCommand() const noexcept {
    return *this;
  }
  inline Windows::Foundation::Collections::IObservableVector<Settings::StringFilterViewModel> Filters() const noexcept {
    return Filters_;
  }
  inline hstring Name() const noexcept {
    return hstring(rule_.name());
  }
  inline Windows::Foundation::Collections::IObservableVector<Settings::FilterKeyViewModel> UnusedFilterKeys() const noexcept {
    return UnusedFilterKeys_;
  }

  void save();

private:
  static void onAddFilterRequested(RuleViewModel& self, Windows::Foundation::IInspectable const& param);
  static void onDeleteRequested(
    Mntone::RoxyGlance::ViewModels::Settings::implementation::StringFilterViewModel const& sender,
    Windows::Foundation::IInspectable const& context
  );

private:
  roxyg::settings::Rule& rule_;
  boost::container::small_vector<FilterKey, 3> deleting_keys_;

  Windows::Foundation::Collections::IObservableVector<Settings::StringFilterViewModel> Filters_;
  Windows::Foundation::Collections::IObservableVector<Settings::FilterKeyViewModel> UnusedFilterKeys_;
};

}
