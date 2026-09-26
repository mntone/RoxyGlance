#pragma once

namespace roxyg::logging {

enum class LogLevel: uint_fast8_t {
  kUnknown = 0,
  kTrace = 1,
  kDebug,
  kInfo,
  kNotice,
  kWarn,
  kError,
  kFatal,
};

enum class LogGroup: uint_fast8_t {
  kUnknown = 0,
  kCompiler,
  kContext,
  kDebug,
  kEngine,
  kMonitor,
  kSettings,
  kWin32,
  kWindow,
};

struct Log final {
  SYSTEMTIME datetime;
  winrt::hstring content;
  winrt::hresult hresult;
  LogLevel level;
  LogGroup group;
};

}
