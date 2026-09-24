#include "pch.h"
#include "App.xaml.h"

#include "Views/MainWindow.xaml.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Xaml;

namespace impl {
using namespace ::winrt::Mntone::RoxyGlance::implementation;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::implementation;
using namespace ::winrt::Mntone::RoxyGlance::Views::implementation;
}
}

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
winrt::impl::App::App()
  : MainWindow_(nullptr) {
  winrt::check_hresult(AppDelegate_.initialize());

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException([](winrt::IInspectable const&, winrt::UnhandledExceptionEventArgs const& e) {
    if (IsDebuggerPresent()) {
      auto errorMessage = e.Message();
      __debugbreak();
    }
  });
#endif
}

/// <summary>
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void winrt::impl::App::OnLaunched([[maybe_unused]] winrt::LaunchActivatedEventArgs const& e) {
  showMainWindow();
}

void winrt::impl::App::showMainWindow() {
  winrt::com_ptr<impl::MainWindow> window{MainWindow_};
  if (!window) {
    winrt::com_ptr<impl::LogsViewModel> view_model{make_self<impl::LogsViewModel>()};
    view_model->setLogger(AppDelegate_.logger());

    winrt::com_ptr<impl::MainWindow> new_window{make_self<impl::MainWindow>()};
    new_window->setLogs(view_model.as<ViewModels::LogsViewModel>());

    window = new_window;
    MainWindow_ = new_window;
  }
  window->Activate();
}
