#pragma once
#include "ThreadController.h"

namespace roxyg::win32 {

class WinEventHookController final
  : public ThreadController {
public:
  [[nodiscard]] winrt::hresult start(
    DWORD event_min,
    DWORD event_max,
    WINEVENTPROC proc
  ) noexcept;

  [[nodiscard]] DWORD stop(DWORD timeout = INFINITE) noexcept;
};

}
