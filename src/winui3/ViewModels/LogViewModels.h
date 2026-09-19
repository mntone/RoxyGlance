#pragma once
#include "ViewModels/LogsViewModel.g.h"

#include "../../app/logging/Logger.h"
#include "LogViewModel.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::implementation {

struct LogsViewModel
  : LogsViewModelT<LogsViewModel>
  , public roxyg::utility::ICollectionChangeListener<roxyg::logging::Log> {
public:
  LogsViewModel() noexcept;
  ~LogsViewModel();

  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs() noexcept {
    return Logs_;
  }

  void unsetLogger();
  void setLogger(roxyg::logging::Logger& logger);

private:
  void onCollectionChanged(roxyg::utility::CollectionChange<roxyg::logging::Log> const& change) noexcept override;

private:
  roxyg::logging::Logger* logger_;
  Microsoft::UI::Dispatching::DispatcherQueue dispatcher_;
  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs_;
};

}
