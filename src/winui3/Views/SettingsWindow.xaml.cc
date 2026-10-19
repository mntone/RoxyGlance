#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("Views/SettingsWindow.g.cpp")
#include "Views/SettingsWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Windowing.h>  // requires OverlappedPresenter

#include "RuleEditorPage.xaml.h"
#include "../Resources/SettingsResource.h"
#include "../ViewModels/Settings/initialize.h"

#include <winrt/Microsoft.UI.Interop.h>
#include "../../app/AppResource.h"
#include "../../app/win32/string.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using Color = ::winrt::Windows::UI::Color;
using Colors = ::winrt::Microsoft::UI::Colors;

using namespace ::winrt::Microsoft::UI;
using namespace ::winrt::Microsoft::UI::Windowing;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Controls;
using namespace ::winrt::Microsoft::UI::Xaml::Interop;
using namespace ::winrt::Microsoft::UI::Xaml::Media::Animation;
using namespace ::winrt::Microsoft::Windows::ApplicationModel::Resources;
using namespace ::winrt::Mntone::RoxyGlance::Views;
}

using namespace roxyg::win32;
using namespace roxyg::winui3::viewmodel::string;

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

void SettingsWindow::NavigationViewSelectionChanged(
  NavigationView const& /*sender*/,
  NavigationViewSelectionChangedEventArgs const& args
) {
  NavigationViewItem item{args.SelectedItem().as<NavigationViewItem>()};
  RootFrame().Navigate(
    TypeName{unbox_value<hstring>(item.Tag())},
    nullptr,
    SuppressNavigationTransitionInfo()
  );
}

}  // namespace winrt::Mntone::RoxyGlance::Views::implementation
