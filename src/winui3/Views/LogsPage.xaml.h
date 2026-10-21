#pragma once
#include "Views/LogsPage.g.h"

namespace roxyg::logging { class Logger; }

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct LogsPage: LogsPageT<LogsPage> {
  LogsPage();
  LogsPage(roxyg::logging::Logger& logger);

  inline ViewModels::Logging::LogsViewModel Logs() const noexcept {
    return Logs_;
  }

private:
  ViewModels::Logging::LogsViewModel Logs_;
};

}

namespace winrt::Mntone::RoxyGlance::Views::factory_implementation {

struct LogsPage: LogsPageT<LogsPage, implementation::LogsPage> {};

}
