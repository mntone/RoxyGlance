#pragma once

#include "../logging/LogHelper.h"

namespace roxyg::win32 {

/// <summary>
/// Represents a notification area icon.
/// </summary>
/// <remarks>
/// Only one TrayIcon may be attached per thread.
/// </remarks>
class TrayIcon final {
  TrayIcon(TrayIcon const&) = delete;
  TrayIcon& operator=(TrayIcon const&) = delete;

public:
  explicit TrayIcon(UINT cbmsg) noexcept;
  explicit TrayIcon(UINT cbmsg, HICON hicon) noexcept;
  TrayIcon(UINT cbmsg, HICON hicon, std::nullptr_t, size_t message_len) = delete;
  explicit TrayIcon(UINT cbmsg, HICON hicon, wchar_t const* message_ptr, size_t message_len) noexcept;
#if _DEBUG
  ~TrayIcon() noexcept;
#endif

private:
  explicit TrayIcon(UINT cbmsg, HICON hicon, wchar_t const* message_ptr, size_t message_len, UINT flags) noexcept;

public:
  /// <summary>
  /// Adds the icon to the notification area and associates it with the given window.
  /// </summary>
  /// <param name="hwnd">The window that receives the tray icon's callback message.</param>
  /// <param name="force">Whether to issue NIM_ADD even if the icon is already attached.</param>
  /// <returns>S_OK on success or if already attached to <paramref name="hwnd"/>, or a failure HRESULT.</returns>
  winrt::hresult attach(HWND hwnd, bool force = false) noexcept;

  /// <summary>
  /// Removes the icon from the notification area.
  /// </summary>
  /// <returns>S_OK if detached or already not attached, or a failure HRESULT.</returns>
  winrt::hresult detach() noexcept;

  winrt::hresult restoreFocusToNotificationArea() const noexcept;
  winrt::hresult showMenu(HMENU hmenu, HWND hwnd, WPARAM wparam, DWORD thread_id) noexcept;

#if _DEBUG
  void validateIconId(LPARAM lparam) const noexcept;
#endif

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

  [[nodiscard]] static winrt::hresult initialize() noexcept;

private:
  winrt::hresult tryUnhookMessageProc(HHOOK const hhook) noexcept;

  static LRESULT __stdcall menuMessageProc(int code, WPARAM wparam, LPARAM lparam) noexcept;

private:
  HWND hwnd_;
  HICON const hicon_;
  wchar_t const* const message_ptr_;
  size_t const message_len_;
  HHOOK hhook_;
  UINT const cbmsg_, flags_;
  mutable logging::LogHelper<logging::LogGroup::kWin32> logger_;

  static thread_local TrayIcon* that_;

public:
  static UINT kTaskbarCreatedWindowCommand;
};

}
