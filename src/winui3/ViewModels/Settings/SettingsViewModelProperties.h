#pragma once

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct SettingsViewModelProperties {
  static void initialize();

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs HasPendingChangesPropertyChangedEventArgs_;
};

}
