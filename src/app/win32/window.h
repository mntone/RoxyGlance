#pragma once

#include "../numeric/numeric.h"

namespace roxyg::win32 {

ROXYG_ALWAYS_INLINE bool IsRootWindow(HWND hwnd) noexcept {
  HWND hRootWnd = GetAncestor(hwnd, GA_ROOT);
  return hRootWnd == hwnd;
}

extern winrt::hresult GetWindowShadowMargin(HWND hwnd, numeric::long4& shadowMargin) noexcept;

}
