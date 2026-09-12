#include "pch.h"
#include "LogViewModels.h"
#if __has_include("ViewModels.LogViewModels.g.cpp")
#include "ViewModels.LogViewModels.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;

namespace impl {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::implementation;
}
}

using namespace magic_enum;
using namespace roxyg;

winrt::impl::LogsViewModel::LogsViewModel() noexcept
  : logger_(nullptr)
  , dispatcher_(DispatcherQueue::GetForCurrentThread())
  , Logs_(winrt::single_threaded_observable_vector<ViewModels::LogViewModel>()) {
}

winrt::impl::LogsViewModel::~LogsViewModel() {
  unsetLogger();
}

void winrt::impl::LogsViewModel::onCollectionChanged(utility::CollectionChange<logging::Log> const& change) {
#if _DEBUG
  assert(!dispatcher_.HasThreadAccess());
#endif

  dispatcher_.TryEnqueue([that = get_weak(), change] {
    winrt::impl::com_ref<winrt::impl::LogsViewModel> viewModel{that.get()};
    if (enum_flags_test(change.type, utility::CollectionChangeType::kAdded)) {
      viewModel->Logs_.InsertAt(change.index, make<impl::LogViewModel>(*change.item));
    }
    if (enum_flags_test(change.type, utility::CollectionChangeType::kRemoved)) {
      viewModel->Logs_.RemoveAtEnd();
    }
  });
}

void winrt::impl::LogsViewModel::unsetLogger() {
  logging::Logger* logger = logger_;
  if (!logger) {
    return;
  }

  logger_ = nullptr;
  logger->removeListener(this);
}

void winrt::impl::LogsViewModel::setLogger(logging::Logger& logger) {
#if _DEBUG
  assert(dispatcher_.HasThreadAccess());
#endif

  unsetLogger();
  auto const& logs{logger.Logs()};

  std::vector<ViewModels::LogViewModel> winrt_logs;
  winrt_logs.reserve(logs.size());
  for (logging::Log const& log : logs) {
    winrt_logs.emplace_back(make<impl::LogViewModel>(log));
  }
  Logs_.ReplaceAll(std::move(winrt_logs));

  logger_ = &logger;
  logger.addListener(this);
}
