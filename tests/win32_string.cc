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

#ifdef _DEBUG
TEST(StringConversion, AssertsOnZeroLengthUtf8Input) {
  std::wstring utf16 = L"previous value";

  EXPECT_DEATH(ConvertUtf8ToUtf16("", 0, utf16), "utf8len > 0 && utf8ptr");
}
#endif

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

TEST(StringConversion, ConvertsUtf16ToUtf8) {
  std::wstring const utf16 = L"utf16to8\U0001F389";
  std::string utf8;

  EXPECT_EQ(ConvertUtf16ToUtf8(
    utf16.data(), static_cast<int>(utf16.size()), utf8), S_OK);
  EXPECT_EQ(utf8, "utf16to8\xF0\x9F\x8E\x89");
}

#ifdef _DEBUG
TEST(StringConversion, AssertsOnZeroLengthUtf16Input) {
  std::string utf8 = "previous value";

  EXPECT_DEATH(ConvertUtf16ToUtf8(L"", 0, utf8), "utf16len > 0 && utf16ptr");
}
#endif

TEST(StringConversion, RejectsMalformedUtf16) {
  std::wstring utf16;
  utf16.push_back(static_cast<wchar_t>(0xD800));
  utf16.push_back(L'x');
  std::string utf8 = "previous value";

  HRESULT const hr = ConvertUtf16ToUtf8(
    utf16.data(), static_cast<int>(utf16.size()), utf8);

  EXPECT_EQ(hr, HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
  EXPECT_EQ(utf8, "previous value");
}

TEST(StringConversion, RejectsTruncatedUtf16) {
  std::wstring utf16;
  utf16.push_back(static_cast<wchar_t>(0xD83C));
  std::string utf8 = "previous value";

  HRESULT const hr = ConvertUtf16ToUtf8(
    utf16.data(), static_cast<int>(utf16.size()), utf8);

  EXPECT_EQ(hr, HRESULT_FROM_WIN32(ERROR_NO_UNICODE_TRANSLATION));
  EXPECT_EQ(utf8, "previous value");
}

}  // namespace test::roxyg::win32
