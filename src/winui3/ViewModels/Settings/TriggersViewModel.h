#pragma once
#include "ViewModels/Settings/TriggersViewModel.g.h"
#include "SettingsViewModelProperties.h"

#include "../Holders/CallbackHolder.h"
#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct TriggersViewModel
  : TriggersViewModelT<TriggersViewModel, non_agile>
  , roxyg::winui3::viewmodel::CallbackHolder<>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<TriggersViewModel>
  , SettingsViewModelProperties {
  TriggersViewModel(
    roxyg::settings::Rule& rule,
    CallbackFunction callback,
    IInspectable const& context
  );

  DEFINE_PROP(bool, ApplicationStarted);
  DEFINE_PROP(bool, WindowActivated);
  DEFINE_PROP(bool, WindowShown);

  inline bool HasPendingChanges() const noexcept {
    return HasPendingChanges_;
  }
  bool applyChanges();

  static void initialize();

private:
  void HasPendingChanges(bool value);

private:
  roxyg::settings::Rule& rule_;

  bool HasPendingChanges_;
  bool ApplicationStarted_;
  bool WindowActivated_;
  bool WindowShown_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ApplicationStartedPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs WindowActivatedPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs WindowShownPropertyChangedEventArgs_;
};

}
