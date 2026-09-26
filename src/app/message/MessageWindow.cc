#include "pch.h"
#include "MessageWindow.h"
#include "MessageWindowResource.h"

#include "../win32/hresult.h"

namespace {

inline constexpr UINT kMessageWindowMessageTrayCommand = WM_APP + 1;
inline constexpr wchar_t kMessageWindowClass[] = L"Mntone.RoxyGlance.MessageWindow";

}

using namespace roxyg::message;

void detail::hmenu_deleter::operator()(HMENU hmenu) const {
  if (hmenu) {
    DestroyMenu(hmenu);
  }
}

detail::unique_hmenu Window::hmenu_{nullptr};

Window::Window() noexcept
  : WindowController()
  , hinstance_(nullptr)
  , tray_icon_{
    kMessageWindowMessageTrayCommand,
  } {
}

winrt::hresult Window::initialize() noexcept {
  if (hmenu_) {
    return S_OK;
  }

  winrt::hresult hr = WindowController::initialize();
  if (FAILED(hr)) {
    return hr;
  }

  HINSTANCE const hinstance{hInstance()};
  ROXYG_UNCHECKED_ASSERT(hinstance);

  HMENU const hmenu = LoadMenuW(hinstance, MAKEINTRESOURCEW(menu::kTray));
  if (!hmenu) {
    return E_FAIL;
  }

  hmenu_.reset(hmenu);
  return S_OK;
}

winrt::hresult Window::start() noexcept {
  return win32::WindowController::start(kMessageWindowClass);
}

LRESULT Window::windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
  switch (message) {
  case kMessageWindowMessageTrayCommand:
#if _DEBUG
    tray_icon_.validateIconId(lparam);
#endif
    switch (LOWORD(lparam)) {
    case WM_CONTEXTMENU:
    {
      HMENU const popup_menu = GetSubMenu(hmenu_.get(), 0);
      if (popup_menu) {
        tray_icon_.showMenu(popup_menu, hwnd, wparam, threadInfo().thread_id);
      }
      return 0;
    }
    case WM_LBUTTONUP:
      notify(&IMessageListener::onShowSettingsRequested);
      return 0;
    }
    break;
  case WM_COMMAND:
    switch (LOWORD(wparam)) {
    case command::kTrayExit:
      tray_icon_.restoreFocusToNotificationArea();
      notify(&IMessageListener::onAppExitRequested);
      return 0;
    case command::kTraySettings:
      tray_icon_.restoreFocusToNotificationArea();
      notify(&IMessageListener::onShowSettingsRequested);
      return 0;
    }
    break;
  case WM_CREATE:
  {
    winrt::hresult const hr = tray_icon_.attach(hwnd);
    if (FAILED(hr)) {
      SetLastError(roxyg::win32::hresult::HResultToWin32(hr));
      return -1;
    }
    return 0;
  }
  case WM_DESTROY:
  {
    winrt::hresult const hr = tray_icon_.detach();
    PostQuitMessage(hr);
    return 0;
  }
  case WM_CLOSE:
    DestroyWindow(hwnd);
    return 0;
  }
  return DefWindowProcW(hwnd, message, wparam, lparam);
}
