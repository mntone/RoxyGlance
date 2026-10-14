#pragma once
#include "Views/MainWindow.g.h"

#include <winrt/Mntone.RoxyGlance.ViewModels.h>

namespace roxyg::logging { class Logger; }

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct MainWindow: MainWindowT<MainWindow> {
  MainWindow();
  MainWindow(roxyg::logging::Logger& logger);

  void MainWindow_Closed(
    winrt::Windows::Foundation::IInspectable const& sender,
    winrt::Microsoft::UI::Xaml::WindowEventArgs const& args
  ) noexcept;

  inline ViewModels::LogsViewModel Logs() const noexcept {
    return Logs_;
  }

private:
  ViewModels::LogsViewModel Logs_;
};

}
