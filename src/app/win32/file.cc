#include "pch.h"
#include "file.h"
#include "hresult.h"

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
    return hresult::LastErrorAsHResult();
  }

  // GetFileSize does not reset the last-error code on success.
  SetLastError(ERROR_SUCCESS);

  wil::unique_hfile file(hfile);
  DWORD file_size = GetFileSize(hfile, nullptr);
  if (file_size == INVALID_FILE_SIZE) {
    DWORD const lasterr = WINRT_IMPL_GetLastError();
    if (lasterr != ERROR_SUCCESS) {
      return hresult::HResultFromWin32(lasterr);
    }
  }

#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  try {
    winrt::hresult hr = S_OK;
    content.resize_and_overwrite(file_size, [&hr, hfile](char* buf, size_t bufSize) -> size_t {
      DWORD bytes_read;
      BOOL const rc = ::ReadFile(hfile, buf, static_cast<DWORD>(bufSize), &bytes_read, nullptr);
      if (rc == FALSE) {
        hr = hresult::LastErrorAsHResult();
        return 0;
      }

      return static_cast<size_t>(bytes_read);
    });
    return hr;
  } catch (std::bad_alloc const&) {
    return E_OUTOFMEMORY;
  } catch (std::length_error const&) {
    return E_INVALIDARG;
  }
#else
  try {
    content.resize(file_size);

    DWORD bytes_read;
    BOOL const rc = ::ReadFile(hfile, content.data(), file_size, &bytes_read, nullptr);
    if (rc == FALSE) {
      content.resize(0);
      return hresult::LastErrorAsHResult();
    }

    content.resize(bytes_read);
    return S_OK;
  } catch (std::bad_alloc const&) {
    return E_OUTOFMEMORY;
  } catch (std::length_error const&) {
    return E_INVALIDARG;
  }
#endif
}
