#pragma once
#include "ViewModels/LogViewModel.g.h"

#include "../app/logging/Log.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::implementation {

struct LogViewModel: LogViewModelT<LogViewModel> {
public:
  LogViewModel() noexcept = default;

  LogViewModel(roxyg::logging::Log const& log) noexcept
    : Content_(log.content) {
  }

  hstring Content() const noexcept {
    return Content_;
  }

private:
  winrt::hstring Content_;
};

}
