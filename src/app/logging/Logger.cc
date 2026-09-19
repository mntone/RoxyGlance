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

static_assert(std::is_nothrow_move_constructible_v<Log>);
static_assert(std::is_nothrow_move_assignable_v<Log>);

Logger::Logger() noexcept
  : logs_(kLogSize) {
}

void Logger::addLog(Log log) noexcept {
  bool const is_full = logs_.full();
  logs_.push_front(std::move(log));

  CollectionChange change{
    is_full
      ? CollectionChangeType::kUpdated
      : CollectionChangeType::kAdded,
    0,
    &logs_.front(),
  };
  notify(&ICollectionChangeListener<Log>::onCollectionChanged, std::move(change));
}
