#include "pch.h"
#include "settings_shared.h"
#include "app/settings/NodeHelper.h"

#include <winrt/base.h>

namespace test::roxyg::settings {

using namespace ::roxyg::settings;

// ---[ Long ]---------------------------------------------

TEST(NodeHelper, ReadLong) {
  EXPECT_EQ(loadFromYaml("0", ReadLongFromNode, INT32_MIN, INT32_MAX), 0);
  EXPECT_EQ(loadFromYaml("2147483647", ReadLongFromNode, INT32_MIN, INT32_MAX), 2147483647);
  EXPECT_EQ(loadFromYaml("-2147483648", ReadLongFromNode, INT32_MIN, INT32_MAX), -2147483648);
  EXPECT_THROW(loadFromYaml("", ReadLongFromNode, INT32_MIN, INT32_MAX), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("2147483648", ReadLongFromNode, INT32_MIN, INT32_MAX), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-2147483649", ReadLongFromNode, INT32_MIN, INT32_MAX), winrt::hresult_invalid_argument);

  EXPECT_EQ(loadFromYaml("4", ReadLongFromNode, 0, 16), 4);
  EXPECT_THROW(loadFromYaml("17", ReadLongFromNode, 0, 16), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1", ReadLongFromNode, 0, 16), winrt::hresult_invalid_argument);

  {
    TEST_YAML(nullval, "value: null");
    EXPECT_THROW(ReadLongFromNode(nullval["value"], INT32_MIN, INT32_MAX), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(missing, "{}");
    EXPECT_THROW(ReadLongFromNode(missing["missing"], INT32_MIN, INT32_MAX), winrt::hresult_invalid_argument);
  }
}

TEST(NodeHelper, ReadLongOrDefault) {
  EXPECT_EQ(loadFromYaml("0", ReadLongFromNodeOrDefault, 0, 16, 4), 0);
  EXPECT_EQ(loadFromYaml("16", ReadLongFromNodeOrDefault, 0, 16, 4), 16);
  EXPECT_THROW(loadFromYaml("17", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);

  EXPECT_EQ(loadFromYaml("null", ReadLongFromNodeOrDefault, 0, 16, 4), 4);  // nullval
  EXPECT_EQ(loadFromYaml("", ReadLongFromNodeOrDefault, 0, 16, 4), 4);      // emptyval

  EXPECT_THROW(loadFromYaml("{}", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);       // emptymap
  EXPECT_THROW(loadFromYaml("[]", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);       // emptyseq
  EXPECT_THROW(loadFromYaml("\"\"", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);     // emptystr
  EXPECT_THROW(loadFromYaml("invalid", ReadLongFromNodeOrDefault, 0, 16, 4), winrt::hresult_invalid_argument);  // str

  {
    TEST_YAML(missing, "{}");
    EXPECT_EQ(ReadLongFromNodeOrDefault(missing["missing"], 0, 16, 4), 4);
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
    EXPECT_EQ(ReadLongFromNode(valueonly, -100, 100), 42);

    WriteLongToNode(valueonly, -17);
    EXPECT_EQ(ReadLongFromNode(valueonly, -100, 100), -17);
  }

  {
    constexpr c4::csubstr key = "value";
    TEST_YAML(keyvalue, "value: 0");

    c4::yml::NodeRef value{keyvalue[key]};
    WriteLongToNode(value, 442);
    EXPECT_EQ(ReadLongFromNode(value, -1000, 1000), 442);
  }

  {
    constexpr c4::csubstr key = "missing";
    TEST_YAML(root, "key: value");

    WriteLongToNode(root[key], 442);
    EXPECT_EQ(ReadLongFromNode(root[key], -1000, 1000), 442);
  }
}


// ---[ Float ]--------------------------------------------

TEST(NodeHelper, ReadFloat) {
  EXPECT_EQ(loadFromYaml("0.5", ReadFloatFromNode, -FLT_MAX, FLT_MAX), 0.5f);
  EXPECT_EQ(loadFromYaml("1.2", ReadFloatFromNode, -FLT_MAX, FLT_MAX), 1.2f);
  EXPECT_EQ(loadFromYaml("-49.5", ReadFloatFromNode, -FLT_MAX, FLT_MAX), -49.5f);
  EXPECT_THROW(loadFromYaml(".nan", ReadFloatFromNode, -FLT_MAX, FLT_MAX), winrt::hresult_invalid_argument);

  EXPECT_EQ(loadFromYaml("0.25", ReadFloatFromNode, 0.f, 1.f), 0.25f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadFloatFromNode, 0.f, 1.f), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1.001", ReadFloatFromNode, 0.f, 1.f), winrt::hresult_invalid_argument);

  {
    TEST_YAML(nullval, "value: null");
    EXPECT_THROW(ReadFloatFromNode(nullval["value"], 0.f, 1.f), winrt::hresult_invalid_argument);
  }

  {
    TEST_YAML(missing, "{}");
    EXPECT_THROW(ReadFloatFromNode(missing["missing"], -1000.f, +1000.f), winrt::hresult_invalid_argument);
  }
}

TEST(NodeHelper, ReadFloatOrDefault) {
  EXPECT_EQ(loadFromYaml("0.5", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), 0.5f);
  EXPECT_THROW(loadFromYaml("-0.001", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml("-1.001", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);
  EXPECT_THROW(loadFromYaml(".nan", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);

  EXPECT_EQ(loadFromYaml("null", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), 0.5f);  // nullval
  EXPECT_EQ(loadFromYaml("", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), 0.5f);      // emptyval

  EXPECT_THROW(loadFromYaml("{}", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);       // emptymap
  EXPECT_THROW(loadFromYaml("[]", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);       // emptyseq
  EXPECT_THROW(loadFromYaml("\"\"", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);     // emptystr
  EXPECT_THROW(loadFromYaml("invalid", ReadFloatFromNodeOrDefault, 0.f, 1.f, 0.5f), winrt::hresult_invalid_argument);  // str

  {
    TEST_YAML(missing, "{}");
    EXPECT_EQ(ReadFloatFromNodeOrDefault(missing["missing"], 0.f, 1.f, 0.5f), 0.5f);
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
    EXPECT_FLOAT_EQ(ReadFloatFromNode(valueonly, -100.f, +100.f), 0.75f);

    WriteFloatToNode(valueonly, -49.5f);
    EXPECT_FLOAT_EQ(ReadFloatFromNode(valueonly, -100.f, +100.f), -49.5f);
  }

  {
    constexpr c4::csubstr key = "value";
    TEST_YAML(keyvalue, "value: 0");

    c4::yml::NodeRef value{keyvalue[key]};
    WriteFloatToNode(value, 442.f);
    EXPECT_EQ(ReadFloatFromNode(value, -1000.f, +1000.f), 442.f);
  }

  {
    constexpr c4::csubstr key = "missing";
    TEST_YAML(root, "key: value");

    WriteFloatToNode(root[key], 442.f);
    EXPECT_EQ(ReadFloatFromNode(root[key], -1000.f, +1000.f), 442.f);
  }
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

TEST(NodeHelper, WriteStringAndCompareType) {
  using CT = settings::StringCompareType;

  c4::yml::Tree tree;
  c4::yml::parse_in_arena(c4::to_csubstr(std::string_view{"{}"}), &tree);
  c4::yml::NodeRef root = tree.rootref();
  std::wstring const value = L"utf16to8\U0001F389";
  c4::csubstr const key = c4::to_csubstr("filter");

  WriteStringAndCompareTypeToNode(root, key, {CT::kContains, value});
  EXPECT_EQ(ReadStringFromNode(root[key]), "utf16to8\xF0\x9F\x8E\x89");
  WriteStringAndCompareTypeToNode(root, key, {CT::kStartsWith, value});
  EXPECT_EQ(ReadStringFromNode(root[key]), "^utf16to8\xF0\x9F\x8E\x89");
  WriteStringAndCompareTypeToNode(root, key, {CT::kEndsWith, value});
  EXPECT_EQ(ReadStringFromNode(root[key]), "utf16to8\xF0\x9F\x8E\x89$");
  WriteStringAndCompareTypeToNode(root, key, {CT::kEquals, value});
  EXPECT_EQ(ReadStringFromNode(root[key]), "^utf16to8\xF0\x9F\x8E\x89$");

  WriteStringAndCompareTypeToNode(root, key, {CT::kNone, value});
  EXPECT_FALSE(root.has_child(key));
}

}  // namespace test::roxyg::settings
