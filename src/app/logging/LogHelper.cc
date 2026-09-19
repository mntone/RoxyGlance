#include "pch.h"
#include "LogHelper.h"

static SYSTEMTIME GetCurrentSystemTime() noexcept {
  SYSTEMTIME st;
  GetSystemTime(&st);
  return st;
}

using namespace roxyg::logging;

void LogHelperBase::log(LogLevel level, LogGroup group, winrt::hstring content, winrt::hresult hresult) noexcept {
  Logger* const logger{logger_};
  if (!logger) {
    return;
  }

  Log log{
    .datetime = GetCurrentSystemTime(),
    .content = std::move(content),
    .hresult = hresult,
    .level = level,
    .group = group,
  };
  logger->addLog(std::move(log));
}
