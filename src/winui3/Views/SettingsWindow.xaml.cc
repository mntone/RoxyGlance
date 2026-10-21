#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("Views/SettingsWindow.g.cpp")
#include "Views/SettingsWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Windowing.h>  // requires OverlappedPresenter

#include "RuleEditorPage.xaml.h"
#include "../Helpers/KeyboardModifiers.h"
#include "../Resources/SettingsResource.h"
#include "../ViewModels/Settings/initialize.h"

#include <winrt/Microsoft.UI.Interop.h>
#include "../../app/AppResource.h"
#include "../../app/win32/string.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::System;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using Color = ::winrt::Windows::UI::Color;
using Colors = ::winrt::Microsoft::UI::Colors;

using namespace ::winrt::Microsoft::UI;
using namespace ::winrt::Microsoft::UI::Input;
using namespace ::winrt::Microsoft::UI::Windowing;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Controls;
using namespace ::winrt::Microsoft::UI::Xaml::Input;
using namespace ::winrt::Microsoft::UI::Xaml::Interop;
using namespace ::winrt::Microsoft::UI::Xaml::Media::Animation;
using namespace ::winrt::Microsoft::UI::Xaml::Navigation;
using namespace ::winrt::Microsoft::Windows::ApplicationModel::Resources;
using namespace ::winrt::Mntone::RoxyGlance::Views;
}

using namespace roxyg::win32;
using namespace roxyg::winui3::string;

namespace winrt::Mntone::RoxyGlance::Views::implementation {

SettingsWindow::SettingsWindow() noexcept {
  ViewModels::Settings::implementation::initialize();

  OverlappedPresenter presenter{OverlappedPresenter::Create()};
  presenter.PreferredMinimumWidth(500);
  presenter.PreferredMinimumHeight(320);

  winrt::AppWindow app_window{AppWindow()};
  app_window.SetPresenter(presenter);
  app_window.TitleBar().ExtendsContentIntoTitleBar(true);
  app_window.ResizeClient({800, 720});
  SetTitleBar(TitleBarControl());

  HICON const hicon{LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(roxyg::icon::kAppMain))};
  IconId const icon_id{GetIconIdFromIcon(hicon)};
  app_window.SetIcon(icon_id);

  DisplayArea const display_area{DisplayArea::GetFromWindowId(app_window.Id(), DisplayAreaFallback::Primary)};
  if (display_area) {
    Windows::Graphics::RectInt32 const area{display_area.OuterBounds()};
    Windows::Graphics::RectInt32 const work_area{display_area.WorkArea()};
    app_window.Move(Windows::Graphics::PointInt32{
      std::max(work_area.X, (area.Width >> 1) - 400),
      std::max(work_area.Y, (area.Height >> 1) - 360),
    });
  }
}

void SettingsWindow::InitializeComponent() {
  if (_contentLoaded) {
    return;
  }
  SettingsWindowT::InitializeComponent();

  RootFrame().CacheSize(2);

  hstring const title{LoadResourceString(kSettingsWindowTitle)};
  Title(title);

  AppWindowTitleBar titlebar{AppWindow().TitleBar()};
  TitleBar const titlebar_control{TitleBarControl()};
  if (!titlebar.ExtendsContentIntoTitleBar()) [[unlikely]] {
    titlebar_control.Visibility(Visibility::Collapsed);
    return;
  }

  titlebar_control.Title(title);
#ifdef _DEBUG
  titlebar_control.Subtitle(LoadResourceString(kSettingsWindowDevelop));
#endif

  bool const is_dark{titlebar_control.ActualTheme() == ElementTheme::Dark};
  IReference<Color> theme_color{is_dark ? Colors::White() : Colors::Black()};
  Color transparent{Colors::Transparent()};

  titlebar.ButtonBackgroundColor(transparent);
  titlebar.ButtonForegroundColor(theme_color);
  titlebar.ButtonHoverBackgroundColor(
    is_dark
    ? ColorHelper::FromArgb(15, 255, 255, 255)
    : ColorHelper::FromArgb(10, 0, 0, 0)
  );
  titlebar.ButtonHoverForegroundColor(theme_color);
  titlebar.ButtonInactiveBackgroundColor(transparent);
  titlebar.ButtonInactiveForegroundColor(
    is_dark
    ? ColorHelper::FromArgb(0, 99, 99, 99)
    : ColorHelper::FromArgb(0, 139, 139, 139)
  );
  titlebar.ButtonPressedBackgroundColor(
    is_dark
    ? ColorHelper::FromArgb(30, 255, 255, 255)
    : ColorHelper::FromArgb(20, 0, 0, 0)
  );
  titlebar.ButtonPressedForegroundColor(theme_color);
}

bool SettingsWindow::navigateTo(hstring page_type) {
  Frame const frame{RootFrame()};
  if (frame.CurrentSourcePageType().Name == page_type) {
    return false;
  }

  return frame.Navigate(
    TypeName{page_type},
    nullptr,
    SuppressNavigationTransitionInfo()
  );
}

bool SettingsWindow::tryGoBack() {
  Frame const frame{RootFrame()};
  if (frame.CanGoBack()) {
    frame.GoBack();
    return true;
  }
  return false;
}

bool SettingsWindow::tryGoForward() {
  Frame const frame{RootFrame()};
  if (frame.CanGoForward()) {
    frame.GoForward();
    return true;
  }
  return false;
}

void SettingsWindow::RootGridKeyDown(IInspectable const& sender, KeyRoutedEventArgs const& args) {
  roxyg::winui3::KeyboardModifiers const states;
  if (!states.isAltOnlyPressed()) {
    return;
  }

  VirtualKey const key{args.Key()};
  if (key == VirtualKey::Left) {
    if (tryGoBack()) {
      args.Handled(true);
    }
  } else if (key == VirtualKey::Right) {
    if (tryGoForward()) {
      args.Handled(true);
    }
  }
}

void SettingsWindow::RootGridPointerPressed(IInspectable const& sender, PointerRoutedEventArgs const& args) {
  PointerPointProperties properties{args.GetCurrentPoint(Content()).Properties()};
  if (properties.IsXButton1Pressed()) {
    if (tryGoBack()) {
      args.Handled(true);
    }
  } else if (properties.IsXButton2Pressed()) {
    if (tryGoForward()) {
      args.Handled(true);
    }
  }
}

void SettingsWindow::NavigationViewBackRequested(
  NavigationView const& /*sender*/,
  NavigationViewBackRequestedEventArgs const& /*args*/
) {
  tryGoBack();
}

void SettingsWindow::NavigationViewSelectionChanged(
  NavigationView const& /*sender*/,
  NavigationViewSelectionChangedEventArgs const& args
) {
  NavigationViewItem item{args.SelectedItem().as<NavigationViewItem>()};
  hstring const page_type{unbox_value<hstring>(item.Tag())};
  navigateTo(page_type);
}

void SettingsWindow::RootFrameNavigated(IInspectable const& /*sender*/, NavigationEventArgs const& args) {
  for (IInspectable const& item : NavigationViewControl().MenuItems()) {
    NavigationViewItem const& navitem{item.as<NavigationViewItem>()};
    hstring const tag{unbox_value<hstring>(navitem.Tag())};
    if (args.SourcePageType().Name == tag) {
      navitem.IsSelected(true);
      break;
    }
  }
}

}  // namespace winrt::Mntone::RoxyGlance::Views::implementation
