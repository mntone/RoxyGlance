#pragma once

namespace roxyg::win32::hresult {

inline constexpr ::winrt::hresult kErrorFileNotFound = ::winrt::impl::hresult_from_win32(ERROR_FILE_NOT_FOUND);
inline constexpr ::winrt::hresult kErrorAccessDenied = ::winrt::impl::hresult_from_win32(ERROR_ACCESS_DENIED);
inline constexpr ::winrt::hresult kErrorNotEnoughMemory = ::winrt::impl::hresult_from_win32(ERROR_NOT_ENOUGH_MEMORY);
inline constexpr ::winrt::hresult kErrorSharingViolation = ::winrt::impl::hresult_from_win32(ERROR_SHARING_VIOLATION);
inline constexpr ::winrt::hresult kErrorAlreadyExists = ::winrt::impl::hresult_from_win32(ERROR_ALREADY_EXISTS);
inline constexpr ::winrt::hresult kErrorOperationAborted = ::winrt::impl::hresult_from_win32(ERROR_OPERATION_ABORTED);
inline constexpr ::winrt::hresult kErrorTimeout = ::winrt::impl::hresult_from_win32(ERROR_TIMEOUT);
inline constexpr ::winrt::hresult kErrorInvalidUserBuffer = ::winrt::impl::hresult_from_win32(ERROR_INVALID_USER_BUFFER);
inline constexpr ::winrt::hresult kErrorNotEnoughQuota = ::winrt::impl::hresult_from_win32(ERROR_NOT_ENOUGH_QUOTA);
inline constexpr ::winrt::hresult kErrorInvalidOperation = ::winrt::impl::hresult_from_win32(ERROR_INVALID_OPERATION);

ROXYG_ALWAYS_INLINE constexpr ::winrt::hresult HResultFromWin32(DWORD val) noexcept {
  return ::winrt::impl::hresult_from_win32(val);
}

ROXYG_ALWAYS_INLINE ::winrt::hresult LastErrorAsHResult() noexcept {
  return HResultFromWin32(WINRT_IMPL_GetLastError());
}

ROXYG_ALWAYS_INLINE constexpr DWORD HResultToWin32(::winrt::hresult hr, DWORD defaultLastError = ERROR_CAN_NOT_COMPLETE) noexcept {
  if (HRESULT_FACILITY(hr) == FACILITY_WIN32) {
    return HRESULT_CODE(hr);
  }

  return defaultLastError;
}

}
