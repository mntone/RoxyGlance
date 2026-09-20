#pragma once

namespace roxyg::win32::hresult {

inline constexpr ::winrt::hresult kErrorTimeout = ::winrt::impl::hresult_from_win32(ERROR_TIMEOUT);
inline constexpr ::winrt::hresult kErrorNotEnoughQuota = ::winrt::impl::hresult_from_win32(ERROR_NOT_ENOUGH_QUOTA);
inline constexpr ::winrt::hresult kErrorInvalidOperation = ::winrt::impl::hresult_from_win32(ERROR_INVALID_OPERATION);

__forceinline constexpr ::winrt::hresult HResultFromWin32(DWORD val) noexcept {
  return ::winrt::impl::hresult_from_win32(val);
}

__forceinline ::winrt::hresult LastErrorAsHResult() noexcept {
  return HResultFromWin32(WINRT_IMPL_GetLastError());
}

}
