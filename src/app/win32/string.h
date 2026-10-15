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

namespace detail {
winrt::hstring FormatResourceStringImpl(UINT fmt_id, std::wformat_args const args);
}

template<typename... Args>
winrt::hstring FormatResourceString(UINT fmt_id, Args&&... args) {
  return detail::FormatResourceStringImpl(fmt_id, std::make_wformat_args(args...));
}

ROXYG_ALWAYS_INLINE winrt::hstring LoadResourceString(UINT resource_id) {
  wchar_t const* ptr;
  int const len = LoadStringW(nullptr, resource_id, reinterpret_cast<LPWSTR>(&ptr), 0);
  if (len <= 0) {
    winrt::throw_last_error();
  }

  return winrt::hstring{ptr, static_cast<winrt::hstring::size_type>(len)};
}

}
