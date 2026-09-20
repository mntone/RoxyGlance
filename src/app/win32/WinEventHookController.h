#pragma once
#include "ThreadController.h"

#include "../logging/LogHelper.h"

namespace roxyg::win32 {

class WinEventHookController final
  : public ThreadController {
public:
  [[nodiscard]] winrt::hresult start(
    DWORD event_min,
    DWORD event_max,
    WINEVENTPROC wndproc
  ) noexcept;

  [[nodiscard]] winrt::hresult stop(DWORD timeout = INFINITE) noexcept;

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

private:
  logging::LogHelper<logging::LogGroup::kWin32> logger_;
};

}
