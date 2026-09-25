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
#if _DEBUG
  ~TrayIcon() noexcept;
#endif

private:
  explicit TrayIcon(UINT cbmsg, HICON hicon, UINT flags) noexcept;

public:
  winrt::hresult attach(HWND hwnd) noexcept;
  winrt::hresult detach() noexcept;

  winrt::hresult restoreFocusToNotificationArea() const noexcept;
  winrt::hresult showMenu(HMENU hmenu, HWND hwnd, WPARAM wparam, DWORD thread_id) noexcept;

#if _DEBUG
  void validateIconId(LPARAM lparam) const noexcept;
#endif

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

private:
  winrt::hresult tryUnhookMessageProc(HHOOK const hhook) noexcept;

  static LRESULT __stdcall menuMessageProc(int code, WPARAM wparam, LPARAM lparam) noexcept;

private:
  HWND hwnd_;
  HICON const hicon_;
  HHOOK hhook_;
  UINT const cbmsg_, flags_;
  mutable logging::LogHelper<logging::LogGroup::kWin32> logger_;

  static thread_local TrayIcon* that_;
};

}
