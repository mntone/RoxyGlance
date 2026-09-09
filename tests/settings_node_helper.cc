#include "pch.h"
#include "settings_shared.h"
#include "app/settings/NodeHelper.h"

#include <winrt/base.h>

namespace test::roxyg::settings {

using namespace ::roxyg::settings;

// ---[ Long ]---------------------------------------------

TEST(NodeHelper, ReadLong) {
  EXPECT_EQ(loadFromYaml("0", ReadLongFromNode), 0);
  EXPECT_EQ(loadFromYaml("2147483647", ReadLongFromNode), 2147483647);
  EXPECT_EQ(loadFromYaml("-2147483648", ReadLongFromNode), -2147483648);
  EXPECT_THROW(loadFromYaml("2147483648", ReadLongFromNode), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-2147483649", ReadLongFromNode), winrt::hresult_invalid_argument);
}

TEST(NodeHelper, ReadBoundedLong) {
  EXPECT_EQ(loadFromYaml("4", ReadBoundedLongFromNode, 0, 16), 4);
  EXPECT_THROW(loadFromYaml("17", ReadBoundedLongFromNode, 0, 16), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1", ReadBoundedLongFromNode, 0, 16), winrt::hresult_invalid_argument);
}


// ---[ Float ]--------------------------------------------

TEST(NodeHelper, ReadFloat) {
  EXPECT_EQ(loadFromYaml("0.5", ReadFloatFromNode), 0.5f);
  EXPECT_EQ(loadFromYaml("1.2", ReadFloatFromNode), 1.2f);
  EXPECT_EQ(loadFromYaml("-49.5", ReadFloatFromNode), -49.5f);
}

TEST(NodeHelper, ReadBoundedFloat) {
  EXPECT_EQ(loadFromYaml("0.25", ReadBoundedFloatFromNode, 0.f, 1.f), 0.25f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadBoundedFloatFromNode, 0.f, 1.f), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1.001", ReadBoundedFloatFromNode, 0.f, 1.f), winrt::hresult_invalid_argument);
}

TEST(NodeHelper, ReadBoundedFloatOrDefault) {
  EXPECT_EQ(loadFromYaml("", ReadBoundedFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), 0.5f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadBoundedFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1.001", ReadBoundedFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);
}


// ---[ String ]--------------------------------------------

TEST(NodeHelper, ReadString) {
  EXPECT_EQ(loadFromYaml("utf8", ReadStringFromNode), "utf8");
  EXPECT_THROW(loadFromYaml("", ReadStringFromNode), winrt::hresult_invalid_argument);
}

TEST(NodeHelper, ReadStringAsUtf16) {
  EXPECT_EQ(loadFromYaml("", ReadStringAsUtf16FromNode), L"");
  EXPECT_EQ(loadFromYaml("utf8to16\xF0\x9F\x8E\x89", ReadStringAsUtf16FromNode), L"utf8to16\U0001F389");
}

TEST(NodeHelper, ReadStringAndCompareType) {
  using CT = settings::StringCompareType;

  EXPECT_EQ(loadFromYaml("", ReadStringAndCompareTypeFromNode), std::make_pair(CT::kNone, L""));
  EXPECT_EQ(loadFromYaml("contains", ReadStringAndCompareTypeFromNode), std::make_pair(CT::kContains, L"contains"));
  EXPECT_EQ(loadFromYaml("^startswith", ReadStringAndCompareTypeFromNode), std::make_pair(CT::kStartsWith, L"startswith"));
  EXPECT_EQ(loadFromYaml("endswith$", ReadStringAndCompareTypeFromNode), std::make_pair(CT::kEndsWith, L"endswith"));
  EXPECT_EQ(loadFromYaml("^equals$", ReadStringAndCompareTypeFromNode), std::make_pair(CT::kEquals, L"equals"));
}

}  // namespace test::roxyg::settings
