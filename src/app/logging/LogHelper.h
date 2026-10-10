#pragma once
#include "Logger.h"

namespace roxyg::logging {

namespace detail {
class LogHelperBase {
public:
  explicit constexpr LogHelperBase(logging::Logger* logger) noexcept
    : logger_(logger) {
  }

  [[nodiscard]] constexpr logging::Logger* logger() noexcept {
    return logger_;
  }
  [[nodiscard]] constexpr logging::Logger const* logger() const noexcept {
    return logger_;
  }

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_ = logger;
  }

protected:
  void log(LogLevel level, LogGroup group, winrt::hstring content, winrt::hresult hresult) noexcept;

private:
  Logger* logger_;
};
}

template<LogGroup Group>
class LogHelper final: public detail::LogHelperBase {
public:
  constexpr LogHelper() noexcept
    : LogHelperBase(nullptr) {
  }
  LogHelper(std::nullptr_t) = delete;
  explicit constexpr LogHelper(logging::Logger* logger) noexcept
    : detail::LogHelperBase(logger) {
  }

  ROXYG_ALWAYS_INLINE void trace(winrt::hstring content, winrt::hresult hresult = S_FALSE) noexcept {
    log(LogLevel::kTrace, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void debug(winrt::hstring content, winrt::hresult hresult = S_FALSE) noexcept {
    log(LogLevel::kDebug, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void info(winrt::hstring content, winrt::hresult hresult = S_FALSE) noexcept {
    log(LogLevel::kInfo, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void notice(winrt::hstring content, winrt::hresult hresult = S_FALSE) noexcept {
    log(LogLevel::kNotice, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void warn(winrt::hstring content, winrt::hresult hresult = E_FAIL) noexcept {
    log(LogLevel::kWarn, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void error(winrt::hstring content, winrt::hresult hresult = E_FAIL) noexcept {
    log(LogLevel::kError, Group, std::move(content), hresult);
  }
  ROXYG_ALWAYS_INLINE void fatal(winrt::hstring content, winrt::hresult hresult = E_FAIL) noexcept {
    log(LogLevel::kFatal, Group, std::move(content), hresult);
  }
};

}
