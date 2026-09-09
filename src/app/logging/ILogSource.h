#pragma once
#include "Log.h"

namespace roxyg::logging {

using LogCallback = void (*)(void* context, Log const& log);

class ILogSource {
public:
  virtual ~ILogSource() noexcept = 0;

  virtual void addCallback(LogCallback callback, void* context) = 0;
};

}
