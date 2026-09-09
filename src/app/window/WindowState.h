#pragma once

#include "../win32/window.h"
#include "../monitor/MonitorState.h"

namespace roxyg::window {

struct State final {
  explicit inline State(HWND hwnd) noexcept
    : shadow_margin_(numeric::float4::make(0.f, 0.f, 0.f, 0.f))
    , process_id_(0)
    , hwnd_(hwnd)
    , hmonitor_(nullptr)
    , process_id_loaded_(false)
    , shadow_margin_dirty_(true)
    , window_class_loaded_(false)
    , window_title_dirty_(true) {
    window_class_.reserve(256);
  }

  [[nodiscard]] constexpr HWND hWnd() const noexcept { return hwnd_; }
  [[nodiscard]] HMONITOR hMonitor() noexcept;
  [[nodiscard]] DWORD processId() const;
  [[nodiscard]] numeric::float4 shadowMargin();
  [[nodiscard]] std::wstring_view windowClass() const;
  [[nodiscard]] std::wstring_view windowTitle();

  constexpr void invalidateWindowLocation() noexcept {
    hmonitor_ = nullptr;
    shadow_margin_dirty_ = true;
  }
  constexpr void invalidateWindowTitle() noexcept {
    window_title_dirty_ = true;
  }

private:
  numeric::float4 shadow_margin_;
  mutable std::wstring window_class_;
  std::wstring window_title_;
  mutable DWORD process_id_;
  HWND const hwnd_;
  HMONITOR hmonitor_;

  mutable bool process_id_loaded_ : 1;
  bool shadow_margin_dirty_ : 1;
  mutable bool window_class_loaded_ : 1;
  bool window_title_dirty_ : 1;
};

}
