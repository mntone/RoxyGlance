#include "pch.h"
#include "window.h"
#include "hresult.h"

using namespace roxyg::numeric;
using namespace roxyg::win32;

winrt::hresult roxyg::win32::GetWindowShadowMargin(HWND hwnd, numeric::long4& shadowMargin) noexcept {
  long4 outerRect;
  BOOL rc = GetWindowRect(hwnd, reinterpret_cast<RECT*>(&outerRect));
  if (!rc) [[unlikely]] {
    return hresult::LastErrorAsHResult();
  }

  long4 innerRect;
  winrt::hresult hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &innerRect, sizeof(long4));
  if (hr < 0) [[unlikely]] {
    return hr;
  }

  shadowMargin.setXY(innerRect.xy() - outerRect.xy());
  shadowMargin.setZW(shadowMargin.xy() + outerRect.zw() - innerRect.zw());
  return S_OK;
}
