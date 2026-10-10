#include "pch.h"
#include "settings_shared.h"
#include "app/settings/NodeHelper.h"

#include <winrt/base.h>

namespace test::roxyg::settings {

using MT = ::roxyg::settings::StringMatchType;

using namespace ::roxyg::settings;

// ---[ Long ]---------------------------------------------

TEST(NodeHelper, ReadLong) {
  EXPECT_EQ(loadFromYaml("0", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), 0);
  EXPECT_EQ(loadFromYaml("2147483647", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), 2147483647);
  EXPECT_EQ(loadFromYaml("-2147483648", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), -2147483648);
  EXPECT_THROW(loadFromYaml("", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), ParseError);
  EXPECT_THROW(loadFromYaml("2147483648", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), ParseError);
  EXPECT_THROW(loadFromYaml("-2147483649", ReadLongFromNode, KeyId::kNone, INT32_MIN, INT32_MAX), ParseError);

  EXPECT_EQ(loadFromYaml("4", ReadLongFromNode, KeyId::kNone, 0, 16), 4);
  EXPECT_THROW(loadFromYaml("17", ReadLongFromNode, KeyId::kNone, 0, 16), ParseError);
  EXPECT_THROW(loadFromYaml("-1", ReadLongFromNode, KeyId::kNone, 0, 16), ParseError);

  {
    TEST_YAML(nullval, "value: null");
    EXPECT_THROW(ReadLongFromNode(nullval["value"], KeyId::kNone, INT32_MIN, INT32_MAX), ParseError);
  }

  {
    TEST_YAML(missing, "{}");
    EXPECT_THROW(ReadLongFromNode(missing["missing"], KeyId::kNone, INT32_MIN, INT32_MAX), ParseError);
  }
}

TEST(NodeHelper, ReadLongOrDefault) {
  EXPECT_EQ(loadFromYaml("0", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), 0);
  EXPECT_EQ(loadFromYaml("16", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), 16);
  EXPECT_THROW(loadFromYaml("17", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);
  EXPECT_THROW(loadFromYaml("-1", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);

  EXPECT_EQ(loadFromYaml("null", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), 4);  // nullval
  EXPECT_EQ(loadFromYaml("", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), 4);      // emptyval

  EXPECT_THROW(loadFromYaml("{}", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);       // emptymap
  EXPECT_THROW(loadFromYaml("[]", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);       // emptyseq
  EXPECT_THROW(loadFromYaml("\"\"", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);     // emptystr
  EXPECT_THROW(loadFromYaml("invalid", ReadLongFromNodeOrDefault, KeyId::kNone, 0, 16, 4), ParseError);  // str

  {
    TEST_YAML(missing, "{}");
    EXPECT_EQ(ReadLongFromNodeOrDefault(missing["missing"], KeyId::kNone, 0, 16, 4), 4);
  }
}

TEST(NodeHelper, WriteLong) {
  {
    TEST_YAML(emptymap, "{}");
    EXPECT_THROW(WriteLongToNode(emptymap, 16), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(emptyseq, "[]");
    EXPECT_THROW(WriteLongToNode(emptyseq, 16), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(valueonly, "0");

    WriteLongToNode(valueonly, 42);
    EXPECT_EQ(ReadLongFromNode(valueonly, KeyId::kNone, -100, 100), 42);

    WriteLongToNode(valueonly, -17);
    EXPECT_EQ(ReadLongFromNode(valueonly, KeyId::kNone, -100, 100), -17);
  }

  {
    constexpr c4::csubstr key = "value";
    TEST_YAML(keyvalue, "value: 0");

    c4::yml::NodeRef value{keyvalue[key]};
    WriteLongToNode(value, 442);
    EXPECT_EQ(ReadLongFromNode(value, KeyId::kNone, -1000, 1000), 442);
  }

  {
    constexpr c4::csubstr key = "missing";
    TEST_YAML(root, "key: value");

    WriteLongToNode(root[key], 442);
    EXPECT_EQ(ReadLongFromNode(root[key], KeyId::kNone, -1000, 1000), 442);
  }
}


// ---[ Float ]--------------------------------------------

TEST(NodeHelper, ReadFloat) {
  EXPECT_EQ(loadFromYaml("0.5", ReadFloatFromNode, KeyId::kNone, -FLT_MAX, FLT_MAX), 0.5f);
  EXPECT_EQ(loadFromYaml("1.2", ReadFloatFromNode, KeyId::kNone, -FLT_MAX, FLT_MAX), 1.2f);
  EXPECT_EQ(loadFromYaml("-49.5", ReadFloatFromNode, KeyId::kNone, -FLT_MAX, FLT_MAX), -49.5f);
  EXPECT_THROW(loadFromYaml(".nan", ReadFloatFromNode, KeyId::kNone, -FLT_MAX, FLT_MAX), ParseError);

  EXPECT_EQ(loadFromYaml("0.25", ReadFloatFromNode, KeyId::kNone, 0.f, 1.f), 0.25f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadFloatFromNode, KeyId::kNone, 0.f, 1.f), ParseError);
  EXPECT_THROW(loadFromYaml("-1.001", ReadFloatFromNode, KeyId::kNone, 0.f, 1.f), ParseError);

  {
    TEST_YAML(nullval, "value: null");
    EXPECT_THROW(ReadFloatFromNode(nullval["value"], KeyId::kNone, 0.f, 1.f), ParseError);
  }

  {
    TEST_YAML(missing, "{}");
    EXPECT_THROW(ReadFloatFromNode(missing["missing"], KeyId::kNone, -1000.f, +1000.f), ParseError);
  }
}

TEST(NodeHelper, ReadFloatOrDefault) {
  EXPECT_EQ(loadFromYaml("0.5", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), 0.5f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);
  EXPECT_THROW(loadFromYaml("-1.001", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);
  EXPECT_THROW(loadFromYaml(".nan", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);

  EXPECT_EQ(loadFromYaml("null", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), 0.5f);  // nullval
  EXPECT_EQ(loadFromYaml("", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), 0.5f);      // emptyval

  EXPECT_THROW(loadFromYaml("{}", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);       // emptymap
  EXPECT_THROW(loadFromYaml("[]", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);       // emptyseq
  EXPECT_THROW(loadFromYaml("\"\"", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);     // emptystr
  EXPECT_THROW(loadFromYaml("invalid", ReadFloatFromNodeOrDefault, KeyId::kNone, 0.f, 1.f, 0.5f), ParseError);  // str

  {
    TEST_YAML(missing, "{}");
    EXPECT_EQ(ReadFloatFromNodeOrDefault(missing["missing"], KeyId::kNone, 0.f, 1.f, 0.5f), 0.5f);
  }
}

TEST(NodeHelper, WriteFloat) {
  {
    TEST_YAML(emptymap, "{}");
    EXPECT_THROW(WriteFloatToNode(emptymap, 16.f), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(emptyseq, "[]");
    EXPECT_THROW(WriteFloatToNode(emptyseq, 16.f), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(valueonly, "0");

    WriteFloatToNode(valueonly, 0.75f);
    EXPECT_FLOAT_EQ(ReadFloatFromNode(valueonly, KeyId::kNone, -100.f, +100.f), 0.75f);

    WriteFloatToNode(valueonly, -49.5f);
    EXPECT_FLOAT_EQ(ReadFloatFromNode(valueonly, KeyId::kNone, -100.f, +100.f), -49.5f);
  }

  {
    constexpr c4::csubstr key = "value";
    TEST_YAML(keyvalue, "value: 0");

    c4::yml::NodeRef value{keyvalue[key]};
    WriteFloatToNode(value, 442.f);
    EXPECT_EQ(ReadFloatFromNode(value, KeyId::kNone, -1000.f, +1000.f), 442.f);
  }

  {
    constexpr c4::csubstr key = "missing";
    TEST_YAML(root, "key: value");

    WriteFloatToNode(root[key], 442.f);
    EXPECT_EQ(ReadFloatFromNode(root[key], KeyId::kNone, -1000.f, +1000.f), 442.f);
  }
}


// ---[ String ]--------------------------------------------

TEST(NodeHelper, ReadString) {
  EXPECT_EQ(loadFromYaml("", ReadStringFromNode), L"");
  EXPECT_EQ(loadFromYaml("utf8to16\xF0\x9F\x8E\x89", ReadStringFromNode), L"utf8to16\U0001F389");

  EXPECT_EQ(loadFromYaml("null", ReadStringFromNode), L"");  // nullval
  EXPECT_EQ(loadFromYaml("", ReadStringFromNode), L"");      // emptyval
  EXPECT_THROW(loadFromYaml("{}", ReadStringFromNode), winrt::hresult_invalid_argument);  // emptymap
  EXPECT_THROW(loadFromYaml("[]", ReadStringFromNode), winrt::hresult_invalid_argument);  // emptyseq
}

TEST(NodeHelper, ReadStringAndMatchType) {
  EXPECT_EQ(loadFromYaml("contains", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kContains, L"contains"));
  EXPECT_EQ(loadFromYaml("^startswith", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kStartsWith, L"startswith"));
  EXPECT_EQ(loadFromYaml("endswith$", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kEndsWith, L"endswith"));
  EXPECT_EQ(loadFromYaml("^equals$", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kEquals, L"equals"));

  EXPECT_EQ(loadFromYaml("", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kContains, L""));
  EXPECT_EQ(loadFromYaml("^", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kStartsWith, L""));
  EXPECT_EQ(loadFromYaml("$", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kEndsWith, L""));
  EXPECT_EQ(loadFromYaml("^$", ReadStringAndMatchTypeFromNode), std::make_pair(MT::kEquals, L""));
}

TEST(NodeHelper, WriteStringAndMatchType) {
  constexpr c4::csubstr key = "value";
  TEST_YAML(root, "{}");

  std::wstring const value = L"utf16to8\U0001F389";

  WriteStringAndMatchTypeToNode(root[key], {MT::kContains, value});
  EXPECT_EQ(root[key].val(), "utf16to8\xF0\x9F\x8E\x89");
  WriteStringAndMatchTypeToNode(root[key], {MT::kStartsWith, value});
  EXPECT_EQ(root[key].val(), "^utf16to8\xF0\x9F\x8E\x89");
  WriteStringAndMatchTypeToNode(root[key], {MT::kEndsWith, value});
  EXPECT_EQ(root[key].val(), "utf16to8\xF0\x9F\x8E\x89$");
  WriteStringAndMatchTypeToNode(root[key], {MT::kEquals, value});
  EXPECT_EQ(root[key].val(), "^utf16to8\xF0\x9F\x8E\x89$");

  WriteStringAndMatchTypeToNode(root[key], {MT::kContains, L""});
  EXPECT_EQ(root[key].val(), "");
  EXPECT_EQ(ReadStringAndMatchTypeFromNode(root[key]), std::make_pair(MT::kContains, L""));

  WriteStringAndMatchTypeToNode(root[key], {MT::kStartsWith, L""});
  EXPECT_EQ(root[key].val(), "^");
  EXPECT_EQ(ReadStringAndMatchTypeFromNode(root[key]), std::make_pair(MT::kStartsWith, L""));

  WriteStringAndMatchTypeToNode(root[key], {MT::kEndsWith, L""});
  EXPECT_EQ(root[key].val(), "$");
  EXPECT_EQ(ReadStringAndMatchTypeFromNode(root[key]), std::make_pair(MT::kEndsWith, L""));

  WriteStringAndMatchTypeToNode(root[key], {MT::kEquals, L""});
  EXPECT_EQ(root[key].val(), "^$");
  EXPECT_EQ(ReadStringAndMatchTypeFromNode(root[key]), std::make_pair(MT::kEquals, L""));
}

}  // namespace test::roxyg::settings
