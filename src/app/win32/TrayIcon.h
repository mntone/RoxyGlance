#pragma once
#include "hresult.h"

#include "../logging/LogHelper.h"

namespace roxyg::win32 {

namespace detail {

struct TrayIconConfig final {
  UINT flags, callback_message;
  HICON icon_handle;
  wchar_t const* message_ptr;
  size_t message_len;
};

class TrayIconBase {
  TrayIconBase(TrayIconBase const&) = delete;
  TrayIconBase& operator=(TrayIconBase const&) = delete;

public:
  TrayIconBase() noexcept;
#if _DEBUG
  ~TrayIconBase() noexcept;
#endif

public:
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

  static winrt::hresult initialize() noexcept;

protected:
  winrt::hresult attach(TrayIconConfig const config, HWND hwnd, bool force) noexcept;

private:
  winrt::hresult tryUnhookMessageProc(HHOOK const hhook) noexcept;

  static LRESULT __stdcall menuMessageProc(int code, WPARAM wparam, LPARAM lparam) noexcept;

private:
  HWND hwnd_;
  HHOOK hhook_;
  mutable logging::LogHelper<logging::LogGroup::kWin32> logger_;

  static thread_local TrayIconBase* that_;

public:
  static UINT kTaskbarCreatedWindowCommand;
};

}

/// <summary>
/// Represents a notification area icon.
/// </summary>
/// <remarks>
/// Only one TrayIcon may be attached per thread.
/// </remarks>
template<
  UINT CallbackMessage,
  WORD IconId = 0,
  wchar_t const* MessagePtr = nullptr,
  size_t MessageLen = MessagePtr == nullptr ? 0 : std::char_traits<wchar_t>::length(MessagePtr)
>
class TrayIcon final: public detail::TrayIconBase {
  static_assert(MessageLen < 128 /*sizeof(NOTIFYICONDATAW::szTip) / sizeof(wchar_t)*/,
    "MessageLen must leave room for the terminating null character in the tray tip.");
  static_assert(MessagePtr != nullptr || MessageLen == 0,
    "MessageLen must be zero when MessagePtr is null.");

public:
  /// <summary>
  /// Adds the icon to the notification area and associates it with the given window.
  /// </summary>
  /// <param name="hwnd">The window that receives the tray icon's callback message.</param>
  /// <param name="force">Whether to issue NIM_ADD even if the icon is already attached.</param>
  /// <returns>S_OK on success or if already attached to <paramref name="hwnd"/>, or a failure HRESULT.</returns>
  winrt::hresult attach(HWND hwnd, bool force = false) noexcept {
    UINT flags{0x01 /*NIF_MESSAGE*/};
    if constexpr (IconId != 0) {
      if (!icon_handle_) {
        return E_UNEXPECTED;
      }
      flags |= 0x02 /* NIF_ICON */;
    }
    if constexpr (MessagePtr != nullptr) {
      flags |= 0x04 /* NIF_TIP */ | 0x80 /* NIF_SHOWTIP */;
    }

    detail::TrayIconConfig const config{
      .flags = flags,
      .callback_message = CallbackMessage,
      .icon_handle = icon_handle_,
      .message_ptr = MessagePtr,
      .message_len = MessageLen,
    };
    return detail::TrayIconBase::attach(config, hwnd, force);
  }

  [[nodiscard]] static winrt::hresult initialize(HINSTANCE hinstance) noexcept {
    if constexpr (IconId != 0) {
      HICON icon_handle{icon_handle_};
      if (!icon_handle) {
        icon_handle = LoadIconW(hinstance, MAKEINTRESOURCE(IconId));
        if (!icon_handle) {
          return hresult::LastErrorAsHResult();
        }

        icon_handle_ = icon_handle;
      }
    }

    return detail::TrayIconBase::initialize();
  }

private:
  inline static HICON icon_handle_{nullptr};
};

}
