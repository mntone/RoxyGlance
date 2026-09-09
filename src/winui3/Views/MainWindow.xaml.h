#pragma once
#include "Views/MainWindow.g.h"

#include "ViewModels/LogViewModels.h"

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct MainWindow: MainWindowT<MainWindow> {
  MainWindow() noexcept;

  constexpr Mntone::RoxyGlance::ViewModels::LogsViewModel Logs() noexcept {
    return Logs_;
  }

  void Window_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);

private:
  Mntone::RoxyGlance::ViewModels::LogsViewModel Logs_{nullptr};
};

}
