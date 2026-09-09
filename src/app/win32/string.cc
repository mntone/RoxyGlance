#include "pch.h"
#include "string.h"

using namespace roxyg;

winrt::hresult win32::ConvertUtf8ToUtf16(
  char const* utf8ptr,
  int const utf8len,
  std::wstring& utf16
) noexcept {
  if (utf8len == 0) [[unlikely]] {
    utf16.clear();
    return S_OK;
  }

  size_t const utf16len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8ptr, utf8len, nullptr, 0);
  if (utf16len == 0) [[unlikely]] {
    utf16.clear();
    return S_OK;
  }

#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  winrt::hresult hr = S_OK;
  utf16.resize_and_overwrite(utf16len, [&hr, utf8ptr, utf8len](wchar_t* buf, size_t buf_size) noexcept {
    int const written = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        utf8ptr, utf8len,
        buf, static_cast<int>(buf_size)
    );
    if (written <= 0) {
      hr = winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
      return 0;
    }

    return written;
  });
  return hr;
#else
  utf16.resize(utf16len);
  int const written = MultiByteToWideChar(
      CP_UTF8, 0,
      utf8ptr, utf8len,
      utf16.data(), utf16len
  );
  if (written <= 0) {
    utf16.resize(0);
    return winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
  }

  utf16.resize(written);
  return S_OK;
#endif
}
