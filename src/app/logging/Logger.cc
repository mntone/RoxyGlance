#include "pch.h"
#include "Logger.h"

namespace {

#if _DEBUG
inline constexpr size_t kLogSize = 256;
#else
inline constexpr size_t kLogSize = 64;
#endif

}

using namespace roxyg::logging;
using namespace roxyg::utility;

Logger::Logger() noexcept
  : logs_(kLogSize) {
}

void Logger::addLog(Log log) {
  bool is_full = logs_.full();
  logs_.push_front(log);

  CollectionChange change{
    is_full
      ? CollectionChangeType::kUpdated
      : CollectionChangeType::kAdded,
    0,
    &logs_.front(),
  };
  notify(&ICollectionChangeListener<Log>::onCollectionChanged, std::move(change));
}
