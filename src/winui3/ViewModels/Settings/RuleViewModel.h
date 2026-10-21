#pragma once
#include "ViewModels/Settings/RuleViewModel.g.h"
#include "SettingsViewModelProperties.h"

#ifdef __INTELLISENSE__
#include <boost/container/small_vector.hpp>
#endif

#include "../Holders/CallbackHolder.h"
#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../Holders/StaticCommandHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct RuleViewModel
  : RuleViewModelT<RuleViewModel, Microsoft::UI::Xaml::Input::ICommand, non_agile>
  , roxyg::winui3::viewmodel::CallbackHolder<>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<RuleViewModel>
  , roxyg::winui3::viewmodel::StaticCommandHolder<RuleViewModel>
  , SettingsViewModelProperties {
  RuleViewModel(
    roxyg::settings::Rule& rule,
    CallbackFunction callback,
    IInspectable const& context
  );

  inline Settings::ActionViewModel Action() const noexcept {
    return Action_;
  }

  inline Settings::ActionType ActionType() const noexcept {
    if (!Action_) {
      return Settings::ActionType::Invalid;
    }
    return Action_.Type();
  }
  void ActionType(Settings::ActionType);

  inline Microsoft::UI::Xaml::Input::ICommand AddFilterCommand() const noexcept {
    return *this;
  }
  inline Windows::Foundation::Collections::IObservableVector<Settings::StringFilterViewModel> Filters() const noexcept {
    return Filters_;
  }
  inline hstring Name() const noexcept {
    return Name_;
  }
  inline Settings::TriggersViewModel Triggers() const noexcept {
    return Triggers_;
  }
  inline Windows::Foundation::Collections::IObservableVector<Settings::FilterKeyViewModel> UnusedFilterKeys() const noexcept {
    return UnusedFilterKeys_;
  }

  bool HasPendingChanges() const noexcept;
  bool applyChanges();

  static void initialize();

private:
  void notifyHasPendingChangesPropertyChanged();

  static void onAddFilterRequested(RuleViewModel& self, Windows::Foundation::IInspectable const& param);
  static void onChildHasPendingChangesUpdated(Windows::Foundation::IInspectable const& context);
  static void onDeleteRequested(
    Windows::Foundation::IInspectable const& context,
    FilterKey key
  );

private:
  roxyg::settings::Rule& rule_;
  boost::container::small_vector<FilterKey, 3> deleting_keys_;

  hstring Name_;
  Settings::ActionViewModel Action_;
  Windows::Foundation::Collections::IObservableVector<Settings::StringFilterViewModel> Filters_;
  Settings::TriggersViewModel const Triggers_;
  Windows::Foundation::Collections::IObservableVector<Settings::FilterKeyViewModel> UnusedFilterKeys_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ActionPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ActionTypePropertyChangedEventArgs_;
};

}
