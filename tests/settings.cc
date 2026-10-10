#include "pch.h"
#include "settings_shared.h"

#include "app/settings/constants.h"
#include "app/settings/NodeHelper.h"
#include "app/settings/Rule.h"
#include "app/settings/action/MoveAndResizeAction.h"

using namespace std::literals::string_view_literals;

namespace test::roxyg::settings {

using AT = ::roxyg::settings::ActionType;
using TF = ::roxyg::settings::TriggerFlags;
using MT = ::roxyg::settings::StringMatchType;

using namespace ::roxyg::settings;
using namespace ::roxyg::settings::action;

// ---[ Action ]-------------------------------------------

TEST(Action_Read, MissingTypeReturnsInvalid) {
  TEST_YAML(root, "x: 0");

  AT value = AT::kRelativeMoveAndResize;
  HRESULT const hr = ReadActionTypeFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, AT::kInvalid);
}

TEST(Action_Read, ReadsTypeCaseInsensitively) {
  TEST_YAML(root, "type: aBsolute_move_and_resize");

  AT value = AT::kRelativeMoveAndResize;
  HRESULT const hr = ReadActionTypeFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, AT::kAbsoluteMoveAndResize);
}

TEST(Action_Read, UnknownTypeReturnsInvalid) {
  TEST_YAML(root, "type: unsupported_action");

  AT value = AT::kRelativeMoveAndResize;
  HRESULT const hr = ReadActionTypeFromNode(root, &value);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(value, AT::kInvalid);
}

TEST(Action_Read, RejectsNonScalarType) {
  TEST_YAML(root, "type: {nested: value}");

  AT value = AT::kRelativeMoveAndResize;
  HRESULT const hr = ReadActionTypeFromNode(root, &value);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(value, AT::kRelativeMoveAndResize);
  EXPECT_EQ(root["type"]["nested"].val(), c4::to_csubstr("value"));
}

TEST(Action_Read, RejectsNonMapNode) {
  TEST_YAML(root, "not a map");

  AT value = AT::kRelativeMoveAndResize;
  HRESULT const hr = ReadActionTypeFromNode(root, &value);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(value, AT::kRelativeMoveAndResize);
  EXPECT_EQ(root.val(), c4::to_csubstr("not a map"));
}

TEST(Action_Write, CreatesType) {
  TEST_YAML(root, "id: 2");

  HRESULT const hr = WriteActionTypeToNode(root, AT::kAbsoluteMoveAndResize);

  EXPECT_EQ(hr, S_OK);
  ASSERT_TRUE(root["type"].is_keyval());
  EXPECT_EQ(root["type"].val(), c4::to_csubstr("absolute_move_and_resize"));
  EXPECT_EQ(root["id"].val(), c4::to_csubstr("2"));
}

TEST(Action_Write, UpdatesTypeAndPreservesOtherFields) {
  TEST_YAML(root, "type: absolute_move_and_resize\nid: 2\nx: 0.25");

  HRESULT const hr = WriteActionTypeToNode(root, AT::kRelativeMoveAndResize);

  EXPECT_EQ(hr, S_OK);
  EXPECT_EQ(root["type"].val(), c4::to_csubstr("relative_move_and_resize"));
  EXPECT_EQ(root["id"].val(), c4::to_csubstr("2"));
  EXPECT_EQ(root["x"].val(), c4::to_csubstr("0.25"));
}

TEST(Action_Write, NoneRemovesTypeAndPreservesOtherFields) {
  TEST_YAML(root, "type: absolute_move_and_resize\nid: 2");

  HRESULT const hr = WriteActionTypeToNode(root, AT::kInvalid);

  EXPECT_EQ(hr, S_OK);
  EXPECT_FALSE(root.has_child(key::kType));
  EXPECT_EQ(root["id"].val(), c4::to_csubstr("2"));

  AT value = AT::kRelativeMoveAndResize;
  EXPECT_EQ(ReadActionTypeFromNode(root, &value), S_OK);
  EXPECT_EQ(value, AT::kInvalid);
}

TEST(Action_Write, RejectsNonMapNode) {
  TEST_YAML(root, "not a map");

  HRESULT const hr = WriteActionTypeToNode(root, AT::kRelativeMoveAndResize);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(root.val(), c4::to_csubstr("not a map"));
}

TEST(Action, AsLoadsMatchingTypeAndRejectsMismatch) {
  TEST_YAML(root, R"(
type: relative_move_and_resize
id: 3
x: 0.25
)");

  Action const action{root};
  EXPECT_EQ(action.type(), AT::kRelativeMoveAndResize);

  auto const detail = action.as<RelativeMoveAndResizeAction>();
  EXPECT_EQ(detail.id(), 3);
  EXPECT_EQ(detail.windowBounds().x(), 0.25f);
  EXPECT_THROW(
    action.as<AbsoluteMoveAndResizeAction>(),
    winrt::hresult_invalid_argument
  );
}


// ---[ Filter ]-------------------------------------------

TEST(Filter, LoadValidFilter) {
  TEST_YAML(root, R"(
process: msedge.exe$
class: ^Intermediate D3D Window$
title: null
)");
  Filter const filter{root};
  EXPECT_EQ(filter.processImageName(), std::make_pair(MT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(filter.windowClass(), std::make_pair(MT::kEquals, L"Intermediate D3D Window"));
  EXPECT_EQ(filter.windowTitle(), std::make_pair(MT::kContains, L""));
}

TEST(Filter, LoadInvalidFilter) {
  {
    TEST_YAML(seqchild, "process: [\"seq is not supported\"]");
    EXPECT_THROW(Filter{seqchild}, winrt::hresult_invalid_argument);
  }
  {
    TEST_YAML(mapchild, R"(
class:
  map: is
  not: supported
)");
    EXPECT_THROW(Filter{mapchild}, winrt::hresult_invalid_argument);
  }
}

TEST(Filter, WriteStringMatchType) {
  TEST_YAML(root, "other: preserved");

  Filter filter{root};
  filter.setProcessImageName({MT::kEndsWith, L"msedge.exe"});
  filter.setWindowClass({MT::kEquals, L"Main Window"});
  filter.setWindowTitle({MT::kStartsWith, L"Settings"});

  EXPECT_EQ(root[key::kProcessKey].val(), c4::to_csubstr("msedge.exe$"));
  EXPECT_EQ(root[key::kWindowClassKey].val(), c4::to_csubstr("^Main Window$"));
  EXPECT_EQ(root[key::kWindowTitleKey].val(), c4::to_csubstr("^Settings"));
  EXPECT_EQ(root["other"].val(), c4::to_csubstr("preserved"));

  Filter const reloaded{root};
  EXPECT_EQ(reloaded.processImageName(), std::make_pair(MT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(reloaded.windowClass(), std::make_pair(MT::kEquals, L"Main Window"));
  EXPECT_EQ(reloaded.windowTitle(), std::make_pair(MT::kStartsWith, L"Settings"));
}

TEST(Filter, WriteEmptyStringMatchType) {
  constexpr c4::csubstr key = "title";
  TEST_YAML(root, "title: ^$");

  Filter filter{root};
  filter.setWindowTitle({MT::kContains, L""});
  EXPECT_FALSE(root.has_child(key));

  filter.setWindowTitle({MT::kEquals, L""});
  ASSERT_TRUE(root.has_child(key));
  EXPECT_EQ(root[key].val(), "^$");
  EXPECT_EQ(Filter{root}.windowTitle(), std::make_pair(MT::kEquals, L""));
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

TEST(Triggers_Read, RejectsMapWhen) {
  TEST_YAML(root, "when: {nested: value}");

  TF value = TF::kWindowFocus;
  HRESULT const hr = ReadTriggerFlagsFromNode(root, &value);

  EXPECT_EQ(hr, E_INVALIDARG);
  EXPECT_EQ(value, TF::kWindowFocus);
  EXPECT_EQ(root["when"]["nested"].val(), c4::to_csubstr("value"));
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

  TEST_YAML(root, R"(
name: "Test Name"
when: [sHoW, INit, invalid]
where:
  process: msedge.exe$
  title: ^Main Browser$
then:
  type: relative_move_and_resize
  width: 0.75
)");
  Rule const rule{root};
  EXPECT_EQ(rule.name(), L"Test Name"sv);
  EXPECT_EQ(rule.triggerFlags(), TF::kApplicationStart | TF::kWindowShow);
  EXPECT_EQ(rule.filter().processImageName(), std::make_pair(MT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(rule.filter().windowTitle(), std::make_pair(MT::kEquals, L"Main Browser"));
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
