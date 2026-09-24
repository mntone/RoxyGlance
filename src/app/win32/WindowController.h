#pragma once
#include "MessageLoopThreadController.h"

#include "../utility/RetryState.h"

namespace roxyg::win32 {

class WindowController
  : public MessageLoopThreadController {
protected:
  WindowController() noexcept;
  WindowController(std::nullptr_t) = delete;

  [[nodiscard]] winrt::hresult start(wchar_t const* class_name) noexcept;

  BOOL postStopMessage(intptr_t target) noexcept override final;

  static ROXYG_ALWAYS_INLINE constexpr HINSTANCE hInstance() noexcept {
    return hinstance_;
  }

public:
  ~WindowController() noexcept override = default;

  [[nodiscard]] winrt::hresult stop() noexcept;

  virtual LRESULT windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept = 0;

private:
  std::atomic<HWND> hwnd_;
  static HINSTANCE hinstance_;
};

}
