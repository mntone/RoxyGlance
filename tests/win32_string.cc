#include "pch.h"
#include <combaseapi.h>
#include "app/win32/string.h"

namespace test::roxyg::win32 {

using namespace ::roxyg::win32;

TEST(StringConversion, ConvertsUtf8ToUtf16) {
  std::string const utf8 = "utf8to16\xF0\x9F\x8E\x89";
  std::wstring utf16;

  EXPECT_EQ(ConvertUtf8ToUtf16(
    utf8.data(), static_cast<int>(utf8.size()), utf16), S_OK);
  EXPECT_EQ(utf16, L"utf8to16\U0001F389");
}

TEST(StringConversion, ConvertsEmptyUtf8ToEmptyUtf16) {
  std::wstring utf16 = L"previous value";

  EXPECT_EQ(ConvertUtf8ToUtf16("", 0, utf16), S_OK);
  EXPECT_TRUE(utf16.empty());
}

TEST(StringConversion, RejectsMalformedUtf8) {
  std::string const utf8{"\xC3(", 2};
  std::wstring utf16 = L"previous value";

  HRESULT const hr = ConvertUtf8ToUtf16(
    utf8.data(), static_cast<int>(utf8.size()), utf16);

  EXPECT_EQ(hr, HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
  EXPECT_EQ(utf16, L"previous value");
}

TEST(StringConversion, RejectsTruncatedUtf8) {
  std::string const utf8{"\xF0\x9F\x8E", 3};
  std::wstring utf16 = L"previous value";

  HRESULT const hr = ConvertUtf8ToUtf16(
    utf8.data(), static_cast<int>(utf8.size()), utf16);

  EXPECT_EQ(hr, HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
  EXPECT_EQ(utf16, L"previous value");
}

}  // namespace test::roxyg::win32
