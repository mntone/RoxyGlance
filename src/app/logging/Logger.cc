#include "pch.h"
#include "Logger.h"

using namespace roxyg::logging;

ILogger::~ILogger() noexcept {}

ILogSource::~ILogSource() noexcept {}

void Logger::addLog(Log log) {
  Logs_.push_back(log);

  for (auto& callback : callbacks_) {
    std::get<0>(callback)(std::get<1>(callback), log);
  }
}
