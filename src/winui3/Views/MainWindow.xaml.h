#pragma once
#include "Views/MainWindow.g.h"

#include "ViewModels/LogViewModels.h"

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct MainWindow: MainWindowT<MainWindow> {
  MainWindow() noexcept;

  inline Mntone::RoxyGlance::ViewModels::LogsViewModel Logs() const noexcept {
    return Logs_;
  }
  inline void setLogs(Mntone::RoxyGlance::ViewModels::LogsViewModel value) noexcept {
    Logs_ = value;
  }

private:
  Mntone::RoxyGlance::ViewModels::LogsViewModel Logs_;
};

}
