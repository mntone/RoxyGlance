#pragma once

namespace roxyg::win32 {

struct ProcessInfo final {
  DWORD id;
  std::wstring filepath;
};

[[nodiscard]] winrt::hresult GetProcessInfoFromHWND(HWND hwnd, ProcessInfo& info) noexcept;

}
