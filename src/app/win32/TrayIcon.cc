#include "pch.h"
#include "TrayIcon.h"

#include <shellapi.h>
#include <windowsx.h>

#include "../win32/hresult.h"

#pragma comment(lib, "Shell32.lib")

struct NOTIFYICONDATAWSubset final {
  DWORD cbSize;
  HWND hWnd;
  UINT uID;
};

static ROXYG_ALWAYS_INLINE constexpr POINT LongPtrToPoint(LONG_PTR lparam) noexcept {
  return POINT{
    GET_X_LPARAM(lparam),
    GET_Y_LPARAM(lparam),
  };
}

namespace {

inline constexpr UINT kTrayIconId = 1;

inline constexpr std::wstring_view kTrayIconAttachWindowMismatch
  = L"Failed to attach the tray icon because it is already attached to a different window.";
inline constexpr std::wstring_view kTrayIconAttachAlreadyExists
  = L"Failed to attach the tray icon because another tray icon is already attached to this thread.";
inline constexpr std::wstring_view kTrayIconAttachFailed
  = L"Failed to add the tray icon to the notification area.";
inline constexpr std::wstring_view kTrayIconSetVersionFailed
  = L"Failed to set the tray icon notification version.";
inline constexpr std::wstring_view kTrayIconAttachCleanupFailed
  = L"Failed to remove the tray icon after notification version setup failed.";
inline constexpr std::wstring_view kTrayIconDetachInvalidOperation
  = L"Failed to detach the tray icon because it is not attached to this thread.";
inline constexpr std::wstring_view kTrayIconDetachFailed
  = L"Failed to delete the tray icon from the notification area.";
inline constexpr std::wstring_view kTrayIconRestoreFocusFailed
  = L"Failed to restore focus to the notification area.";

inline constexpr std::wstring_view kTrayIconHookInstallFailed
  = L"Failed to install the optional tray menu focus hook.";
inline constexpr std::wstring_view kTrayIconHookRemovalFailed
  = L"Failed to remove the tray menu focus hook.";
inline constexpr std::wstring_view kTrayIconPostMessageFailed
  = L"Failed to post a message after showing the tray menu.";
inline constexpr std::wstring_view kTrayIconSetForegroundFailed
  = L"Failed to bring the tray menu owner window to the foreground.";
inline constexpr std::wstring_view kTrayIconShowMenuFailed
  = L"Failed to display the tray menu.";
inline constexpr std::wstring_view kTrayIconShowMenuInvalidMenu
  = L"Failed to show the tray menu because the menu handle is invalid.";

}

using namespace roxyg::win32;

thread_local TrayIcon* TrayIcon::that_{nullptr};

TrayIcon::TrayIcon(UINT cbmsg, HICON hicon, UINT flags) noexcept
  : hwnd_(nullptr)
  , hicon_(hicon)
  , hhook_(nullptr)
  , cbmsg_(cbmsg)
  , flags_(flags) {
}
TrayIcon::TrayIcon(UINT cbmsg) noexcept
  : TrayIcon(cbmsg, nullptr, NIF_MESSAGE) {
}
TrayIcon::TrayIcon(UINT cbmsg, HICON hicon) noexcept
  : TrayIcon(cbmsg, hicon, NIF_MESSAGE | NIF_ICON) {
}

#if _DEBUG
TrayIcon::~TrayIcon() noexcept {
  assert(hwnd_ == nullptr);
}
#endif

winrt::hresult TrayIcon::attach(HWND hwnd) noexcept {
  HWND attached_hwnd{hwnd_};
  if (attached_hwnd) {
    if (attached_hwnd != hwnd) {
      logger_.error(winrt::hstring{kTrayIconAttachWindowMismatch}, E_UNEXPECTED);
      return E_UNEXPECTED;
    }
    return S_FALSE;
  }

  TrayIcon const* current_that{that_};
  if (current_that != nullptr && current_that != this) {
    logger_.error(winrt::hstring{kTrayIconAttachAlreadyExists}, hresult::kErrorAlreadyExists);
    return hresult::kErrorAlreadyExists;
  }

  NOTIFYICONDATAW data{
    .cbSize = sizeof(NOTIFYICONDATAW),
    .hWnd = hwnd,
    .uID = kTrayIconId,
    .uFlags = flags_,
    .uCallbackMessage = cbmsg_,
    .hIcon = hicon_,
    .szTip = 0,
    .dwState = 0,
    .dwStateMask = 0,
    .szInfo = 0,
  };
  data.uVersion = NOTIFYICON_VERSION_4;

  BOOL rc = Shell_NotifyIconW(NIM_ADD, &data);
  if (rc == FALSE) [[unlikely]] {
    logger_.error(winrt::hstring{kTrayIconAttachFailed}, E_FAIL);
    return E_FAIL;
  }

  rc = Shell_NotifyIconW(NIM_SETVERSION, &data);
  if (rc == FALSE) [[unlikely]] {
    rc = Shell_NotifyIconW(NIM_DELETE, &data);
    logger_.error(winrt::hstring{kTrayIconSetVersionFailed}, E_FAIL);
    if (rc == FALSE) [[unlikely]] {
      // Keep the icon attached so detach() can retry the cleanup later.
      hwnd_ = hwnd;
      that_ = this;
      logger_.error(winrt::hstring{kTrayIconAttachCleanupFailed}, E_FAIL);
    }
    return E_FAIL;
  }

  hwnd_ = hwnd;
  that_ = this;
  return S_OK;
}

winrt::hresult TrayIcon::detach() noexcept {
  HWND attached_hwnd{hwnd_};
  if (!attached_hwnd) {
    logger_.error(winrt::hstring{kTrayIconDetachInvalidOperation}, hresult::kErrorInvalidOperation);
    return hresult::kErrorInvalidOperation;
  }

  TrayIcon const* current_that{that_};
  if (current_that == nullptr || current_that != this) {
    logger_.error(winrt::hstring{kTrayIconDetachInvalidOperation}, hresult::kErrorInvalidOperation);
    return hresult::kErrorInvalidOperation;
  }

  NOTIFYICONDATAWSubset data{
    sizeof(NOTIFYICONDATAWSubset),
    attached_hwnd,
    kTrayIconId,
  };

  BOOL const rc = Shell_NotifyIconW(NIM_DELETE, reinterpret_cast<NOTIFYICONDATAW*>(&data));
  if (rc == FALSE) [[unlikely]] {
    logger_.error(winrt::hstring{kTrayIconDetachFailed}, E_FAIL);
    return E_FAIL;
  }

  hwnd_ = nullptr;
  that_ = nullptr;
  return S_OK;
}

winrt::hresult TrayIcon::restoreFocusToNotificationArea() const noexcept {
  NOTIFYICONDATAWSubset data{
    sizeof(NOTIFYICONDATAWSubset),
    hwnd_,
    kTrayIconId,
  };
  BOOL const rc = Shell_NotifyIconW(NIM_SETFOCUS, reinterpret_cast<NOTIFYICONDATAW*>(&data));
  if (rc == FALSE) [[unlikely]] {
    logger_.error(winrt::hstring{kTrayIconRestoreFocusFailed}, E_FAIL);
    return E_FAIL;
  }

  return S_OK;
}

LRESULT __stdcall TrayIcon::menuMessageProc(int code, WPARAM wparam, LPARAM lparam) noexcept {
  TrayIcon const* that{that_};
  if (!that) [[unlikely]] {
    return 0;
  }

  if (code == MSGF_MENU) {
    MSG const& msg{*reinterpret_cast<MSG*>(lparam)};
    if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
      that->restoreFocusToNotificationArea();
    }
  }
  return CallNextHookEx(that->hhook_, code, wparam, lparam);
}

winrt::hresult TrayIcon::tryUnhookMessageProc(HHOOK const hhook) noexcept {
  ROXYG_UNCHECKED_ASSERT(hhook);

  BOOL const rc = UnhookWindowsHookEx(hhook);
  if (rc == FALSE) [[unlikely]] {
    DWORD const lasterr = WINRT_IMPL_GetLastError();
    if (lasterr != ERROR_INVALID_HOOK_HANDLE) {
      winrt::hresult const unhook_hr{hresult::HResultFromWin32(lasterr)};
      logger_.error(winrt::hstring{kTrayIconHookRemovalFailed}, unhook_hr);
      return unhook_hr;
    }
  }

  // The hook was removed or is already invalid; clear the cached handle.
  hhook_ = nullptr;
  return S_OK;
}

winrt::hresult TrayIcon::showMenu(HMENU hmenu, HWND hwnd, WPARAM wparam, DWORD thread_id) noexcept {
  if (!hmenu) [[unlikely]] {
    logger_.error(winrt::hstring{kTrayIconShowMenuInvalidMenu}, E_INVALIDARG);
    return E_INVALIDARG;
  }
#if _DEBUG
  assert(IsMenu(hmenu));
#endif

  winrt::hresult hr = S_OK;
  HHOOK const stale_hook{hhook_};
  if (stale_hook) [[unlikely]] {
    hr = tryUnhookMessageProc(stale_hook);
    if (FAILED(hr)) {
      return hr;
    }
  }

  BOOL rc = SetForegroundWindow(hwnd);
  if (rc == FALSE) [[unlikely]] {
    logger_.error(winrt::hstring{kTrayIconSetForegroundFailed}, E_FAIL);
    return E_FAIL;
  }

  // Optional: failure only disables focus restoration when the menu is dismissed with Escape.
  HHOOK const hhook = SetWindowsHookExW(WH_MSGFILTER, menuMessageProc, nullptr, thread_id);
  if (!hhook) [[unlikely]] {
    winrt::hresult const hook_hr{hresult::LastErrorAsHResult()};
    logger_.warn(winrt::hstring{kTrayIconHookInstallFailed}, hook_hr);
  } else {
    hhook_ = hhook;
  }

  POINT pt{LongPtrToPoint(wparam)};
  rc = TrackPopupMenuEx(hmenu, TPM_LEFTBUTTON, pt.x, pt.y, hwnd, nullptr);
  if (rc == FALSE) [[unlikely]] {
    hr = hresult::LastErrorAsHResult();
    logger_.error(winrt::hstring{kTrayIconShowMenuFailed}, hr);
  }

  if (hhook) [[likely]] {
    winrt::hresult const unhook_hr{tryUnhookMessageProc(hhook)};
    if (FAILED(unhook_hr) && hr == S_OK) {
      hr = unhook_hr;
    }
  }

  rc = PostMessageW(hwnd, WM_NULL, 0, 0);
  if (rc == FALSE) [[unlikely]] {
    winrt::hresult const post_hr{hresult::LastErrorAsHResult()};
    logger_.warn(winrt::hstring{kTrayIconPostMessageFailed}, post_hr);
  }

  return hr;
}

#if _DEBUG
void TrayIcon::validateIconId(LPARAM lparam) const noexcept {
  assert(static_cast<UINT>(HIWORD(lparam)) == kTrayIconId);
}
#endif
