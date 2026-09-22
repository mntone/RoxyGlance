#pragma once
#include "ThreadController.h"

#include "../logging/LogHelper.h"
#include "../utility/RetryState.h"

namespace roxyg::win32 {

class WinEventHookController final
  : public ThreadController {
public:
  WinEventHookController() noexcept;
  WinEventHookController(std::nullptr_t) = delete;
  explicit WinEventHookController(std::unique_ptr<utility::IRetryStateFactory>&& retry_factory) noexcept;

  [[nodiscard]] winrt::hresult start(
    DWORD event_min,
    DWORD event_max,
    WINEVENTPROC wndproc
  ) noexcept;

  [[nodiscard]] winrt::hresult stop() noexcept;

  void setRetryFactory(std::nullptr_t) = delete;
  constexpr void setRetryFactory(std::unique_ptr<utility::IRetryStateFactory>&& retry_factory) noexcept {
    retry_factory_ = std::move(retry_factory);
  }
  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

private:
  std::unique_ptr<utility::IRetryStateFactory> retry_factory_;
  logging::LogHelper<logging::LogGroup::kWin32> logger_;
};

}
