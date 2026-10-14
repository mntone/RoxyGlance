#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("Views/MainWindow.g.cpp")
#include "Views/MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Interop.h>  // requires GetIconIdFromIcon
#include <winrt/Microsoft.UI.Windowing.h>  // requires AppIcon::SetIcon

#include "../AppLocator.h"
#include "../ViewModels/LogsViewModel.h"

#include "../../app/AppResource.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI;
using namespace ::winrt::Microsoft::UI::Windowing;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels;
using namespace ::winrt::Mntone::RoxyGlance::Views;
}

using namespace roxyg::winui3;

namespace winrt::Mntone::RoxyGlance::Views::implementation {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::implementation;

MainWindow::MainWindow(roxyg::logging::Logger& logger)
  : Logs_(make<LogsViewModel>(logger)) {
  HICON const hIcon{LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(roxyg::icon::kAppMain))};
  IconId const iconId{GetIconIdFromIcon(hIcon)};
  AppWindow().SetIcon(iconId);
}

MainWindow::MainWindow()
  : MainWindow(GetAppDelegate().logger()) {
}

void MainWindow::MainWindow_Closed(
  [[maybe_unused]] IInspectable const& sender,
  [[maybe_unused]] WindowEventArgs const& args
) noexcept {
  Bindings->StopTracking();
  Logs_ = nullptr;
}

}  // namespace winrt::Mntone::RoxyGlance::Views
