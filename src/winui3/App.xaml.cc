#include "pch.h"
#include "App.xaml.h"

#include "Views/MainWindow.xaml.h"

#include "../app/message/MessageWindow.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::Views;
}

namespace winrt::Mntone::RoxyGlance::implementation {
using namespace ::winrt::Mntone::RoxyGlance::Views::implementation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
  : dispatcher_(DispatcherQueue::GetForCurrentThread())
  , main_window_(nullptr) {
  DispatcherShutdownMode(DispatcherShutdownMode::OnExplicitShutdown);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(AppDelegate_.messageWindow().addListener(this));
  check_hresult(AppDelegate_.initialize());

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException([](IInspectable const&, UnhandledExceptionEventArgs const& e) {
    if (IsDebuggerPresent()) {
      auto errorMessage = e.Message();
      __debugbreak();
    }
  });
#endif
}

App::~App() noexcept {
  assert(!main_window_closed_token_);
}

/// <summary>
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const& e) {
  showMainWindow();
}

void App::exitApp() {
#ifdef _DEBUG
  assert(dispatcher_.HasThreadAccess());
#endif

  [[maybe_unused]] winrt::hresult const remove_hr = AppDelegate_.messageWindow().removeListener(this);

  winrt::MainWindow window{main_window_};
  if (window) {
    event_token closed_token{main_window_closed_token_};
    if (closed_token) {
      main_window_closed_token_.value = 0;
      window.Closed(closed_token);
    }

    main_window_ = nullptr;
    window.Close();
  }

  [[maybe_unused]] winrt::hresult const exit_hr = AppDelegate_.exit();
  Exit();
}

void App::showMainWindow() {
  winrt::MainWindow window{main_window_};
  if (!window) {
    winrt::MainWindow new_window{make<MainWindow>(AppDelegate_.logger())};
    main_window_closed_token_ = new_window.Closed([that = get_weak()](
      [[maybe_unused]] IInspectable const& sender,
      [[maybe_unused]] WindowEventArgs const& args
      ) noexcept {
      com_ptr<App> app{that.get()};
      event_token token{app->main_window_closed_token_};
      app->main_window_closed_token_.value = 0;

      winrt::MainWindow window{app->main_window_};
      app->main_window_ = nullptr;

      window.Closed(token);
    });

    window = new_window;
    main_window_ = new_window;
  }
  window.Activate();
}

void App::onAppExitRequested() noexcept {
  dispatcher_.TryEnqueue([this] {
    exitApp();
  });
}

void App::onShowSettingsRequested() noexcept {
  dispatcher_.TryEnqueue([this] {
    showMainWindow();
  });
}

}  // namespace winrt::Mntone::RoxyGlance::implementation
