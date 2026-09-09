#pragma once

namespace roxyg::win32 {

extern winrt::hresult ConvertUtf8ToUtf16(
  char const* utf8ptr,
  int utf8len,
  std::wstring& utf16
) noexcept;

}
