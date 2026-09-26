#include "pch.h"
#include "App.xaml.h"

#include "Views/MainWindow.xaml.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Microsoft::UI::Dispatching;
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
  : dispatcher_(DispatcherQueue::GetForCurrentThread())
  , MainWindow_(nullptr) {
  DispatcherShutdownMode(DispatcherShutdownMode::OnExplicitShutdown);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(AppDelegate_.addMessageListener(this));
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

winrt::impl::App::~App() noexcept {
  assert(!main_window_closed_token_);
}

/// <summary>
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void winrt::impl::App::OnLaunched([[maybe_unused]] winrt::LaunchActivatedEventArgs const& e) {
  showMainWindow();
}

void winrt::impl::App::exitApp() {
#if _DEBUG
  assert(dispatcher_.HasThreadAccess());
#endif

  winrt::com_ptr<impl::MainWindow> window{MainWindow_};
  if (window) {
    winrt::event_token closed_token{main_window_closed_token_};
    if (closed_token) {
      main_window_closed_token_.value = 0;
      window->Closed(closed_token);
    }

    MainWindow_ = nullptr;
    window->Close();
  }

  [[maybe_unused]] winrt::hresult const hr = AppDelegate_.exit();
  Exit();
}

void winrt::impl::App::showMainWindow() {
  winrt::com_ptr<impl::MainWindow> window{MainWindow_};
  if (!window) {
    winrt::com_ptr<impl::LogsViewModel> view_model{make_self<impl::LogsViewModel>()};
    view_model->setLogger(AppDelegate_.logger());

    winrt::com_ptr<impl::MainWindow> new_window{make_self<impl::MainWindow>()};
    new_window->setLogs(view_model.as<ViewModels::LogsViewModel>());

    main_window_closed_token_ = new_window->Closed([that = get_weak()](
      [[maybe_unused]] winrt::IInspectable const& sender,
      [[maybe_unused]] winrt::WindowEventArgs const& args
      ) noexcept {
      winrt::impl::com_ref<winrt::impl::App> app{that.get()};
      winrt::event_token token{app->main_window_closed_token_};
      app->main_window_closed_token_.value = 0;

      winrt::com_ptr<impl::MainWindow> window{app->MainWindow_};
      app->MainWindow_ = nullptr;

      window->Closed(token);
    });

    window = new_window;
    MainWindow_ = new_window;
  }
  window->Activate();
}

void winrt::impl::App::onAppExitRequested() noexcept {
  dispatcher_.TryEnqueue([this] {
    exitApp();
  });
}

void winrt::impl::App::onShowSettingsRequested() noexcept {
  dispatcher_.TryEnqueue([this] {
    showMainWindow();
  });
}
