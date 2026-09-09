#pragma once
#include "ILogger.h"
#include "ILogSource.h"

#include "../utility/CallbackBase.hpp"

namespace roxyg::logging {

class Logger final
  : public utility::CallbackBase<LogCallback>
  , public ILogger
  , public ILogSource
  , public std::enable_shared_from_this<Logger> {
  static constexpr size_t LogSize = 256;

  Logger(Logger const&) = delete;
  Logger& operator=(Logger const&) = delete;

public:
  Logger() noexcept = default;
  virtual ~Logger() noexcept = default;

  inline boost::circular_buffer<Log> const Logs() const noexcept {
    return Logs_;
  }

  void addCallback(LogCallback callback, void* context) override {
    utility::CallbackBase<LogCallback>::addCallback(callback, context);
  }
  void addLog(Log log) override;

  std::shared_ptr<ILogger> __getAsLogger() {
    return shared_from_this();
  }

  std::shared_ptr<ILogSource> __getAsLogSource() {
    return shared_from_this();
  }

private:
  boost::circular_buffer<Log> Logs_{ LogSize };
};

}
