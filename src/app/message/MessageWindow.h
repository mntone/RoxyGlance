#pragma once
#include "IMessageListener.h"

#include "../utility/ListenerHost.h"
#include "../win32/TrayIcon.h"
#include "../win32/WindowController.h"

namespace roxyg::message {

namespace detail {
struct hmenu_deleter final {
  void operator()(HMENU hMenu) const;
};
using unique_hmenu = std::unique_ptr<std::remove_pointer_t<HMENU>, hmenu_deleter>;
}

class Window final
  : public win32::WindowController
  , public utility::ListenerHost<IMessageListener> {
public:
  Window() noexcept;
  ~Window() noexcept override = default;

  [[nodiscard]] winrt::hresult start() noexcept;

  LRESULT windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept override;

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
    tray_icon_.setLogger(logger);
  }

  [[nodiscard]] static winrt::hresult initialize() noexcept;

private:
  HINSTANCE hinstance_;
  win32::TrayIcon tray_icon_;
  static detail::unique_hmenu hmenu_;
};

}
