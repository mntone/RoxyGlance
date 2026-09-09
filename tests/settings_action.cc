#include "pch.h"
#include "settings_shared.h"
#include "app/settings/action/MoveAndResizeAction.h"

namespace test::roxyg::settings::action {

using namespace ::roxyg::settings::action;

// ---[ AbsoluteMoveAndResize ]----------------------------

TEST(AbsoluteMoveAndResize, LoadValidYaml) {
  constexpr std::string_view yaml = R"(
x: 0
y: 40
width: 320
height: 240
)";
  auto const action = loadFromYaml<AbsoluteMoveAndResizeAction>(yaml);
  EXPECT_EQ(action.windowBounds().x(), 0);
  EXPECT_EQ(action.windowBounds().y(), 40);
  EXPECT_EQ(action.windowBounds().z(), 320);
  EXPECT_EQ(action.windowBounds().w(), 240);
}

TEST(AbsoluteMoveAndResize, LoadInvalidSmallNumber) {
  constexpr std::string_view yaml = R"(
x: -1
y: 40
width: 320
height: 240
)";
  EXPECT_THROW(
    loadFromYaml<AbsoluteMoveAndResizeAction>(yaml),
    winrt::hresult_invalid_argument);
}

TEST(AbsoluteMoveAndResize, LoadInvalidLargeNumber) {
  constexpr std::string_view yaml = R"(
x: 40
y: 40
width: 16385
height: 240
)";
  EXPECT_THROW(
    loadFromYaml<AbsoluteMoveAndResizeAction>(yaml),
    winrt::hresult_invalid_argument);
}


// ---[ RelativeMoveAndResize ]----------------------------

TEST(RelativeMoveAndResize, LoadValidYaml) {
  constexpr std::string_view yaml = R"(
x: 0
y: 1
width: 0.75
height: 0.9
)";
  auto const action = loadFromYaml<RelativeMoveAndResizeAction>(yaml);
  EXPECT_EQ(action.windowBounds().x(), 0.f);
  EXPECT_EQ(action.windowBounds().y(), 1.0f);
  EXPECT_EQ(action.windowBounds().z(), 0.75f);
  EXPECT_EQ(action.windowBounds().w(), 0.9f);
}

TEST(RelativeMoveAndResize, LoadValidYamlWithoutAll) {
  constexpr std::string_view yaml = R"(
)";
  auto const action = loadFromYaml<RelativeMoveAndResizeAction>(yaml);
  EXPECT_EQ(action.windowBounds().x(), 0.5f);
  EXPECT_EQ(action.windowBounds().y(), 0.5f);
  EXPECT_EQ(action.windowBounds().z(), 0.75f);
  EXPECT_EQ(action.windowBounds().w(), 1.f);
}

TEST(RelativeMoveAndResize, LoadInvalidSmallNumber) {
  constexpr std::string_view yaml = R"(
x: -0.001
y: 0.5
width: 0.75
height: 1
)";
  EXPECT_THROW(
    loadFromYaml<RelativeMoveAndResizeAction>(yaml),
    winrt::hresult_invalid_argument);
}

TEST(RelativeMoveAndResize, LoadInvalidLargeNumber) {
  constexpr std::string_view yaml = R"(
x: 0
y: 0.5
width: 0.75
height: 1.001
)";
  EXPECT_THROW(
    loadFromYaml<RelativeMoveAndResizeAction>(yaml),
    winrt::hresult_invalid_argument);
}

}  // namespace test::roxyg::settings::action
