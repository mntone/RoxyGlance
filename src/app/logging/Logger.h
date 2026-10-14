#pragma once
#include "Log.h"

#include "../utility/ICollectionChangeListener.h"
#include "../utility/ListenerHost.h"

namespace roxyg::logging {

class Logger final
  : public utility::ListenerHost<utility::ICollectionChangeListener<Log>> {
  Logger(Logger const&) = delete;
  Logger& operator=(Logger const&) = delete;

public:
  using ContainerType = boost::circular_buffer<Log>;

  Logger() noexcept;

  void addLog(Log log) noexcept;

  constexpr ContainerType const& Logs() const noexcept { return logs_; }

private:
  ContainerType logs_;
};

}
