#pragma once
#include "LogViewModel.h"
#include "ViewModels/LogsViewModel.g.h"

#include "../../app/utility/ICollectionChangeListener.h"

namespace roxyg::logging {
struct Log;
class Logger;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::implementation {

struct LogsViewModel
  : LogsViewModelT<LogsViewModel, non_agile>
  , roxyg::utility::ICollectionChangeListener<roxyg::logging::Log> {
public:
  LogsViewModel(roxyg::logging::Logger& logger);
  ~LogsViewModel();

  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs() noexcept {
    return Logs_;
  }

private:
  void onCollectionChanged(roxyg::utility::CollectionChange<roxyg::logging::Log> const& change) noexcept override;

private:
  Microsoft::UI::Dispatching::DispatcherQueue dispatcher_;
  roxyg::logging::Logger& logger_;
  Windows::Foundation::Collections::IObservableVector<ViewModels::LogViewModel> Logs_;
};

}
