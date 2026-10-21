#include "pch.h"
#include "LogsViewModel.h"
#if __has_include("ViewModels/Logging/LogsViewModel.g.cpp")
#include "ViewModels/Logging/LogsViewModel.g.cpp"
#endif

#include <magic_enum/magic_enum_flags.hpp>  // requires enum_flags_test

#include "../../app/logging/Logger.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation::Collections;

using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Logging;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Logging::implementation {

static ROXYG_ALWAYS_INLINE IObservableVector<winrt::LogViewModel>
makeLogViewModels(roxyg::logging::Logger& logger) {
  using namespace roxyg::logging;
  Logger::ContainerType const& logs{logger.Logs()};

  std::vector<winrt::LogViewModel> winrt_logs;
  winrt_logs.reserve(logs.size());
  for (Log const& log : logs) {
    winrt_logs.emplace_back(make<LogViewModel>(log));
  }

  return single_threaded_observable_vector<winrt::LogViewModel>(std::move(winrt_logs));
}

LogsViewModel::LogsViewModel(roxyg::logging::Logger& logger)
  : dispatcher_(DispatcherQueue::GetForCurrentThread())
  , logger_(logger)
  , Logs_(makeLogViewModels(logger_)) {
  ROXYG_DEBUG_ASSERT_SUCCEEDED(logger.addListener(this));
}

LogsViewModel::~LogsViewModel() {
  logger_.removeListener(this);
}

void LogsViewModel::onCollectionChanged(
  roxyg::utility::CollectionChange<roxyg::logging::Log> const& change
) noexcept {
  using namespace magic_enum;
  using namespace roxyg::utility;

#ifdef _DEBUG
  assert(!dispatcher_.HasThreadAccess());
#endif

  dispatcher_.TryEnqueue([that = get_weak(), change] {
    impl::com_ref<LogsViewModel> viewmodel{that.get()};
    if (viewmodel) {
      if (enum_flags_test(change.type, CollectionChangeType::kAdded)) {
        winrt::LogViewModel log_viewmodel{make<LogViewModel>(*change.item)};
        viewmodel->Logs_.InsertAt(change.index, log_viewmodel);
      }
      if (enum_flags_test(change.type, CollectionChangeType::kRemoved)) {
        viewmodel->Logs_.RemoveAtEnd();
      }
    }
  });
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::implementation
