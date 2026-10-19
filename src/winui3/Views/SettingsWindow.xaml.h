#pragma once
#include "Views/SettingsWindow.g.h"

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct SettingsWindow: SettingsWindowT<SettingsWindow> {
  SettingsWindow() noexcept;

  void InitializeComponent();

  void RootGridKeyDown(
    Windows::Foundation::IInspectable const& sender,
    Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
  );
  void RootGridPointerPressed(
    Windows::Foundation::IInspectable const& sender,
    Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args
  );

  void NavigationViewBackRequested(
    Microsoft::UI::Xaml::Controls::NavigationView const& sender,
    Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args
  );
  void NavigationViewSelectionChanged(
    Microsoft::UI::Xaml::Controls::NavigationView const& sender,
    Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args
  );

  void RootFrameNavigated(
    Windows::Foundation::IInspectable const& sender,
    Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args
  );

private:
  bool tryGoBack();
  bool tryGoForward();
};

}
