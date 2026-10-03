#pragma once
#include "IMessageListener.h"

#include "../AppResource.h"
#include "../utility/ListenerHost.h"
#include "../win32/TrayIcon.h"
#include "../win32/WindowController.h"

namespace roxyg::message {

namespace detail {
struct hmenu_deleter final {
  void operator()(HMENU hMenu) const;
};
using unique_hmenu = std::unique_ptr<std::remove_pointer_t<HMENU>, hmenu_deleter>;

inline constexpr UINT kMessageWindowMessageTrayCommand = WM_APP + 1;
inline constexpr wchar_t kMessageWindowTrayMessage[] = L"Roxy Glance";
}

class Window final
  : public win32::WindowController
  , public utility::ListenerHost<IMessageListener> {
  using TrayIcon = win32::TrayIcon<
    detail::kMessageWindowMessageTrayCommand,
    icon::kAppMain,
    detail::kMessageWindowTrayMessage
  >;

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
  TrayIcon tray_icon_;
  static detail::unique_hmenu hmenu_;
};

}
