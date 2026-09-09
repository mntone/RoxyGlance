#pragma once

#include "../numeric/numeric.h"

namespace roxyg::win32 {

__forceinline bool IsRootWindow(HWND hwnd) noexcept {
  HWND hRootWnd = GetAncestor(hwnd, GA_ROOT);
  return hRootWnd == hwnd;
}

extern winrt::hresult GetWindowShadowMargin(HWND hwnd, numeric::long4& shadowMargin) noexcept;

}
