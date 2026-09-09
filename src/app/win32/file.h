#pragma once
#include <filesystem>

namespace roxyg::win32 {

namespace hresult {

inline constexpr winrt::hresult kOk = S_OK;
inline constexpr winrt::hresult kFileNotFound = winrt::impl::hresult_from_win32(ERROR_FILE_NOT_FOUND);
inline constexpr winrt::hresult kPathNotFound = winrt::impl::hresult_from_win32(ERROR_PATH_NOT_FOUND);
inline constexpr winrt::hresult kTooManyOpenFiles = winrt::impl::hresult_from_win32(ERROR_TOO_MANY_OPEN_FILES);
inline constexpr winrt::hresult kAccessDenied = winrt::impl::hresult_from_win32(ERROR_ACCESS_DENIED);
inline constexpr winrt::hresult kSharingViolation = winrt::impl::hresult_from_win32(ERROR_SHARING_VIOLATION);
inline constexpr winrt::hresult kInvalidUserBuffer = winrt::impl::hresult_from_win32(ERROR_INVALID_USER_BUFFER);

}

extern winrt::hresult ReadFile(std::filesystem::path filepath, std::string& content) noexcept;

}
