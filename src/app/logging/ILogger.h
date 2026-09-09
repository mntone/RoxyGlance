#pragma once
#include "Log.h"

namespace roxyg::logging {

class ILogger {
public:
  virtual ~ILogger() noexcept = 0;

  virtual void addLog(Log log) = 0;
};

}
