#pragma once
#include "ViewModels/LogsViewModel.g.h"

#include "LogViewModel.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::implementation {

struct LogsViewModel: LogsViewModelT<LogsViewModel> {
public:
  LogsViewModel() noexcept;

  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs() noexcept {
    return Logs_;
  }

private:
  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs_;
};

}
