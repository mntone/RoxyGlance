#pragma once
#include "ViewModels/Settings/TriggersViewModel.g.h"

#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/Rule.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct TriggersViewModel
  : TriggersViewModelT<TriggersViewModel, non_agile>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<TriggersViewModel> {
  TriggersViewModel(roxyg::settings::Rule& rule);

  DEFINE_PROP(bool, ApplicationStarted);
  DEFINE_PROP(bool, WindowActivated);
  DEFINE_PROP(bool, WindowShown);

  void save();

  static void initialize();

private:
  roxyg::settings::Rule& rule_;

  bool dirty_;
  bool ApplicationStarted_;
  bool WindowActivated_;
  bool WindowShown_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ApplicationStartedPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs WindowActivatedPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs WindowShownPropertyChangedEventArgs_;
};

}
