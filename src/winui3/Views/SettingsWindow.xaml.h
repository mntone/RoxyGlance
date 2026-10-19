#pragma once
#include "Views/SettingsWindow.g.h"

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct SettingsWindow: SettingsWindowT<SettingsWindow> {
  SettingsWindow() noexcept;

  void InitializeComponent();

  void NavigationViewSelectionChanged(
    winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender,
    winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args
  );
};

}
