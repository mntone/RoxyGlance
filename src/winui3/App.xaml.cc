#include "pch.h"
#include "App.xaml.h"

#include "Views/MainWindow.xaml.h"

#include <boost/circular_buffer.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/static_string.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "../app/engine/Rule.h"
#include "../app/context/AppContext.h"  // require some boost headers
#include "../app/logging/Logger.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Xaml;

namespace impl {
using namespace ::winrt::Mntone::RoxyGlance::implementation;
using namespace ::winrt::Mntone::RoxyGlance::Views::implementation;
}
}

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
winrt::impl::App::App()
  : window_(nullptr) {
  std::shared_ptr<roxyg::logging::Logger> logger{
    std::make_shared<roxyg::logging::Logger>()
  };
  winrt::check_hresult(AppDelegate_.context().initialize(logger, logger));

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
  window_ = make<impl::MainWindow>();
  window_.Activate();
}
