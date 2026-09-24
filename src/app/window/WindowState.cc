#include "pch.h"
#include "WindowState.h"

using namespace roxyg::window;

DWORD State::processId() const {
  if (process_id_loaded_) [[likely]] {
    return process_id_;
  }

  DWORD processId;
  DWORD const rc = GetWindowThreadProcessId(hwnd_, &processId);
  winrt::check_bool(rc);

  process_id_ = processId;
  process_id_loaded_ = true;
  return process_id_;
}

HMONITOR State::hMonitor() noexcept {
  if (hmonitor_) {
    return hmonitor_;
  }

  hmonitor_ = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
  return hmonitor_;
}

roxyg::numeric::float4 State::shadowMargin() {
  if (!shadow_margin_dirty_) [[likely]] {
    return shadow_margin_;
  }

  numeric::long4 shadowMarginInt;
  winrt::hresult hr = roxyg::win32::GetWindowShadowMargin(hwnd_, shadowMarginInt);
  winrt::check_hresult(hr);

  shadow_margin_ = static_cast<numeric::float4>(shadowMarginInt);
  shadow_margin_dirty_ = false;
  return shadow_margin_;
}

std::wstring_view State::windowClass() const {
  if (window_class_loaded_) [[likely]] {
    return window_class_;
  }

#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  window_class_.resize_and_overwrite(256, [hwnd = hwnd_](wchar_t* buf, size_t bufSize) {
    int const written = GetClassNameW(hwnd, buf, static_cast<int>(bufSize));
    if (written <= 0) {
      winrt::throw_last_error();
    }
    return written;
  });
#else
  window_class_.resize(256);

  int const len = GetClassNameW(hwnd_, window_class_.data(), 256);
  if (len <= 0) {
    window_class_.clear();
    winrt::throw_last_error();
  }

  window_class_.resize(len);
#endif
  window_class_loaded_ = true;
  return window_class_;
}

std::wstring_view State::windowTitle() {
  if (!window_title_dirty_) [[likely]] {
    return window_title_;
  }

  // GetWindowTextLengthW does not reset the last-error code on success.
  SetLastError(ERROR_SUCCESS);

  int const len = GetWindowTextLengthW(hwnd_);
  if (len <= 0) {
    winrt::check_win32(WINRT_IMPL_GetLastError());
    window_title_.clear();
    return window_title_;
  }

#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  window_title_.resize_and_overwrite(len, [hwnd = hwnd_](wchar_t* buf, size_t bufSize) {
    int const written = GetWindowTextW(hwnd, buf, static_cast<int>(bufSize) + 1);
    if (written <= 0) {
      winrt::throw_hresult(E_FAIL);
    }
    return written;
  });
#else
  window_title_.resize(len);
  int const written = GetWindowTextW(hwnd_, window_title_.data(), len + 1);
  if (written <= 0) {
    window_title_.clear();
    winrt::throw_hresult(E_FAIL);
  }
  window_title_.resize(written);
#endif
  window_title_dirty_ = false;
  return window_title_;
}
