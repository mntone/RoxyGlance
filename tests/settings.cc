#include "pch.h"
#include "settings_shared.h"

#include "app/settings/Rule.h"

using namespace std::literals::string_view_literals;

namespace test::roxyg::settings {

using AT = ::roxyg::settings::ActionType;
using TT = ::roxyg::settings::TriggerType;
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
  EXPECT_EQ(filter.processName(), std::make_pair(CT::kEndsWith, L"msedge.exe"));
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
  EXPECT_EQ(filter.processName(), std::make_pair(CT::kNone, L""));
  EXPECT_EQ(filter.windowClass(), std::make_pair(CT::kNone, L""));
  EXPECT_EQ(filter.windowTitle(), std::make_pair(CT::kContains, L"null"));
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
  - type: relative_move_and_resize
    width: 0.75
)";
  auto const rule = loadFromYaml<Rule>(yaml);
  EXPECT_EQ(rule.name(), L"Test Name"sv);
  EXPECT_EQ(rule.trigger(), TT::kApplicationInit | TT::kWindowShow);
  EXPECT_EQ(rule.filter().processName(), std::make_pair(CT::kEndsWith, L"msedge.exe"));
  EXPECT_EQ(rule.filter().windowTitle(), std::make_pair(CT::kEquals, L"Main Browser"));
  EXPECT_EQ(rule.actions()[0].type(), AT::kRelativeMoveAndResize);
}

}  // namespace test::roxyg::settings
