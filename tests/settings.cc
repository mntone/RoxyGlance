#include "pch.h"
#include "settings_shared.h"

#include "app/settings/Rule.h"

using namespace std::literals::string_view_literals;

namespace test::roxyg::settings {

using AT = ::roxyg::settings::ActionType;
using TF = ::roxyg::settings::TriggerFlags;
using CT = ::roxyg::settings::StringCompareType;

using namespace ::roxyg::settings;

// ---[ Action ]-------------------------------------------

TEST(Action, LoadAction) {
  EXPECT_EQ(loadFromYaml<Action>("type: invalid_action_type").type(), AT::kUnknown);
  EXPECT_EQ(loadFromYaml<Action>("type: absolute_move_and_resize").type(), AT::kAbsoluteMoveAndResize);
  EXPECT_EQ(loadFromYaml<Action>("type: relative_move_and_resize").type(), AT::kRelativeMoveAndResize);
}


// ---[ Filter ]-------------------------------------------

TEST(Filter, LoadValidFilter) {
  constexpr std::string_view yaml = R"(
process: msedge.exe$
class: ^Intermediate D3D Window$
title: ^Main Browser$
)";
  auto const filter = loadFromYaml<Filter>(yaml);
  EXPECT_EQ(filter.processImageName(), std::make_pair(CT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(filter.windowClass(), std::make_pair(CT::kEquals, L"Intermediate D3D Window"));
  EXPECT_EQ(filter.windowTitle(), std::make_pair(CT::kEquals, L"Main Browser"));
}

TEST(Filter, LoadInvalidFilter) {
  constexpr std::string_view yaml = R"(
process: ["array is not supported"]
class:
  map: is
  not: supported
title: null
)";
  auto const filter = loadFromYaml<Filter>(yaml);
  EXPECT_EQ(filter.processImageName(), std::make_pair(CT::kNone, L""));
  EXPECT_EQ(filter.windowClass(), std::make_pair(CT::kNone, L""));
  EXPECT_EQ(filter.windowTitle(), std::make_pair(CT::kContains, L"null"));
}


// ---[ Triggers ]-----------------------------------------

TEST(Triggers_Read, MissingWhenReturnsNone) {
  TEST_YAML(root, "name: test");

  TF value = TF::kWindowFocus;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, TF::kNone);
}

TEST(Triggers_Read, ReadsScalarWhen) {
  TEST_YAML(root, "when: FoCuS");

  TF value = TF::kNone;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, TF::kWindowFocus);
}

TEST(Triggers_Read, ReadsSequenceAndIgnoresUnknownEntries) {
  using namespace magic_enum::bitwise_operators;

  TEST_YAML(root, "when: [init, sHoW, invalid, {custom: value}]");

  TF value = TF::kNone;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, TF::kApplicationStart | TF::kWindowShow);
}

TEST(Triggers_Read, TreatsMapWhenAsNone) {
  TEST_YAML(root, "when: {nested: value}");

  TF value = TF::kWindowFocus;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, TF::kNone);
}

TEST(Triggers_Read, RejectsNonMapNode) {
  TEST_YAML(root, "not a map");

  TF value = TF::kWindowFocus;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(value, TF::kWindowFocus);
}

TEST(Triggers_Write, UpdatesSequence) {
  using namespace magic_enum::bitwise_operators;

  TEST_YAML(root, "when: [init, show]");

  TF const expected = TF::kWindowFocus | TF::kWindowShow;
  HRESULT const hr = WriteTriggerFlagsToNode(root, expected);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(vals_to_vec(root["when"]), (TestVec<4>{"show", "focus"}));
}

TEST(Triggers_Write, RemovesDuplicateValues) {
  TEST_YAML(root, "when: [show, show]");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kNone);

  EXPECT_EQ(hr, S_OK);

  c4::yml::ConstNodeRef const when = root["when"];
  ASSERT_TRUE(when.has_key());
  ASSERT_TRUE(when.is_seq());
  EXPECT_EQ(when.num_children(), 0u);
}

TEST(Triggers_Write, AcceptsScalarWhen) {
  TEST_YAML(root, "when: show");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kWindowFocus);

  EXPECT_EQ(hr, S_OK);

  c4::yml::ConstNodeRef const when = root["when"];
  ASSERT_TRUE(when.is_keyval());
  EXPECT_EQ(when.val(), c4::to_csubstr("focus"));
}

TEST(Triggers_Write, PreservesNonScalarWhenEntries) {
  TEST_YAML(root, "when: [show, {custom: value}]");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kWindowFocus);

  EXPECT_EQ(hr, S_OK);

  c4::yml::ConstNodeRef const when = root["when"];
  ASSERT_EQ(when.num_children(), 2u);

  c4::yml::ConstNodeRef const custom_entry = when.first_child();
  ASSERT_TRUE(custom_entry.is_map());
  EXPECT_EQ(custom_entry["custom"].val(), c4::to_csubstr("value"));

  c4::yml::ConstNodeRef const focus_entry = custom_entry.next_sibling();
  ASSERT_TRUE(focus_entry.is_val());
  EXPECT_EQ(focus_entry.val(), c4::to_csubstr("focus"));
}

TEST(Triggers_Write, InsertsWhenBeforeWhere) {
  TEST_YAML(root, "name: test\nwhere: {}\nthen: {}");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kApplicationStart);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(keys_to_vec(root), (TestVec<4>{"name", "when", "where", "then"}));
}

TEST(Triggers_Write, InsertsWhenBeforeThenWhenWhereIsMissing) {
  TEST_YAML(root, "name: test\nthen: {}");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kApplicationStart);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(keys_to_vec(root), (TestVec<4>{"name", "when", "then"}));
}

TEST(Triggers_Write, AppendsWhenWhenWhereAndThenAreMissing) {
  TEST_YAML(root, "name: test");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kApplicationStart);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(keys_to_vec(root), (TestVec<4>{"name", "when"}));
}

TEST(Triggers_Write, RejectsNonMapNode) {
  TEST_YAML(root, "not a map");

  HRESULT const hr = WriteTriggerFlagsToNode(root, TF::kApplicationStart);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(root.val(), c4::to_csubstr("not a map"));
}


// ---[ Rule ]---------------------------------------------

TEST(Rule, LoadValidRule) {
  using namespace magic_enum::bitwise_operators;

  constexpr std::string_view yaml = R"(
name: "Test Name"
when: [sHoW, INit, invalid]
where:
  process: msedge.exe$
  title: ^Main Browser$
then:
  type: relative_move_and_resize
  width: 0.75
)";
  auto const rule = loadFromYaml<Rule>(yaml);
  EXPECT_EQ(rule.name(), L"Test Name"sv);
  EXPECT_EQ(rule.triggerFlags(), TF::kApplicationStart | TF::kWindowShow);
  EXPECT_EQ(rule.filter().processImageName(), std::make_pair(CT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(rule.filter().windowTitle(), std::make_pair(CT::kEquals, L"Main Browser"));
  EXPECT_EQ(rule.action().type(), AT::kRelativeMoveAndResize);
}

TEST(Rule, SetTriggerFlagsUpdatesCachedValueAndYaml) {
  using namespace magic_enum::bitwise_operators;

  TEST_YAML(root, R"(
when: [init, show]
where: {}
then:
  type: absolute_move_and_resize
)");
  Rule rule(root);

  TF const expected = TF::kWindowFocus | TF::kWindowShow;
  rule.setTriggerFlags(expected);

  EXPECT_EQ(rule.triggerFlags(), expected);
  EXPECT_EQ(vals_to_vec(root["when"]), (TestVec<4>{"show", "focus"}));
}

}  // namespace test::roxyg::settings
