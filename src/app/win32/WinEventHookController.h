#pragma once
#include "MessageLoopThreadController.h"

namespace roxyg::win32 {

class WinEventHookController final
  : public MessageLoopThreadController {
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

protected:
  BOOL postStopMessage(intptr_t target) noexcept override final;
};

}
