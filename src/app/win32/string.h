#pragma once

namespace roxyg::win32 {

winrt::hresult ConvertUtf8ToUtf16(
  char const* utf8ptr,
  int utf8len,
  std::wstring& utf16
) noexcept;

winrt::hresult ConvertUtf16ToUtf8(
  wchar_t const* utf16ptr,
  int utf16len,
  std::string& utf8
) noexcept;

}
