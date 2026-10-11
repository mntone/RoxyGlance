#include "pch.h"
#include "settings_shared.h"

#include "app/settings/ParseErrorPrivate.h"

namespace test::roxyg::settings {

using namespace ::roxyg::settings;
using namespace ::roxyg::settings::detail;

TEST(ParseError, MakeRuleDisplayName) {
  {
    TEST_YAML(rule, "name: example");
    ParseError err{};
    err.key_id = KeyId::kName;

    EXPECT_EQ(makeRuleDisplayName(rule, err, 1), L"\"example\"");
  }

  {
    TEST_YAML(rule, "when: focus");
    ParseError err{};
    err.key_id = KeyId::kName;

    EXPECT_EQ(makeRuleDisplayName(rule, err, 2), L"#2");
  }

  {
    TEST_YAML(rule, "name: null");
    ParseError err{};
    err.key_id = KeyId::kName;

    EXPECT_EQ(makeRuleDisplayName(rule, err, 3), L"#3");
  }

  {
    TEST_YAML(rule, "name: invalid\xFF");
    ParseError err{};
    err.key_id = KeyId::kName;

    EXPECT_EQ(makeRuleDisplayName(rule, err, 4), L"#4");
  }

  {
    TEST_YAML(rule, "not a map");
    ParseError err{};
    err.key_id = KeyId::kRule;

    EXPECT_EQ(makeRuleDisplayName(rule, err, 5), L"#5");
  }
}

TEST(ParseError, MakeParseErrorMessage) {
  ParseError expected_string{};
  expected_string.reason = ParseErrorReason::kExpectedString;
  expected_string.key_id = KeyId::kWindowTitle;
  winrt::hstring const expected_string_message{
    makeParseErrorMessage(expected_string, L"\"Main window\"")
  };
  EXPECT_STREQ(expected_string_message.c_str(), L"Key \"title\" must be a string in rule \"Main window\"");

  ParseError out_of_range{};
  out_of_range.reason = ParseErrorReason::kNumberOutOfRange;
  out_of_range.key_id = KeyId::kWidth;
  out_of_range.setContent(42);
  winrt::hstring const out_of_range_message{
    makeParseErrorMessage(out_of_range, L"#1")
  };
  EXPECT_STREQ(out_of_range_message.c_str(), L"Key \"width\" is out of range in rule #1: 42");
}

}
