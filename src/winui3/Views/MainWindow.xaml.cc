#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("Views.MainWindow.g.cpp")
#include "Views.MainWindow.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Xaml;

namespace impl {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::implementation;
using namespace ::winrt::Mntone::RoxyGlance::Views::implementation;
}
}

winrt::impl::MainWindow::MainWindow() noexcept
  : Logs_(nullptr) {
}

void winrt::impl::MainWindow::MainWindow_Closed(
  [[maybe_unused]] winrt::IInspectable const& sender,
  [[maybe_unused]] winrt::WindowEventArgs const& args
) noexcept {
  Bindings->StopTracking();
  Logs_ = nullptr;
}
