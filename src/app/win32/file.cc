#include "pch.h"
#include "file.h"

#include <wil/resource.h>

using namespace roxyg;

winrt::hresult win32::ReadFile(std::filesystem::path filepath, std::string& content) noexcept {
  HANDLE hfile = CreateFileW(
    filepath.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
  );
  if (hfile == INVALID_HANDLE_VALUE) {
    return winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
  }

  wil::unique_hfile file(hfile);
  DWORD fileSize = GetFileSize(hfile, nullptr);
  if (fileSize == INVALID_FILE_SIZE) {
    return winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
  }

#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  winrt::hresult hr = S_OK;
  content.resize_and_overwrite(fileSize, [&hr, hfile](char* buf, size_t bufSize) {
    DWORD bytesRead;
    BOOL const rc = ::ReadFile(hfile, buf, static_cast<DWORD>(bufSize), &bytesRead, nullptr);
    if (!rc) {
      hr = winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
      return 0;
    }

    return static_cast<int>(bytesRead);
  });
  return hr;
#else
  content.resize(fileSize);

  DWORD bytesRead;
  BOOL const rc = ::ReadFile(hfile, content.data(), fileSize, &bytesRead, nullptr);
  if (!rc) {
    content.resize(0);
    return winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
  }

  content.resize(bytesRead);
  return S_OK;
#endif
}
