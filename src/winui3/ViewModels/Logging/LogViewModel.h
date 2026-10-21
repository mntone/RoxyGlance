#pragma once
#include "ViewModels/Logging/LogViewModel.g.h"

#include "../app/logging/Log.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Logging::implementation {

struct LogViewModel: LogViewModelT<LogViewModel, non_agile> {
public:
  LogViewModel() noexcept = default;

  LogViewModel(roxyg::logging::Log const& log) noexcept
    : Content_(log.content) {
  }

  inline hstring Content() const noexcept {
    return Content_;
  }

private:
  winrt::hstring const Content_;
};

}
