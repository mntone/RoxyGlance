#include "pch.h"
#include "settings_shared.h"
#include "app/settings/action/MoveAndResizeAction.h"

namespace test::roxyg::settings::action {

using namespace ::roxyg::settings::action;

// ---[ AbsoluteMoveAndResize ]----------------------------

TEST(AbsoluteMoveAndResize, LoadValidYaml) {
  TEST_YAML(root, R"(
x: 0
y: 40
width: 320
height: 240
)");
  AbsoluteMoveAndResizeAction const action{root};
  EXPECT_EQ(action.windowBounds().x(), 0);
  EXPECT_EQ(action.windowBounds().y(), 40);
  EXPECT_EQ(action.windowBounds().z(), 320);
  EXPECT_EQ(action.windowBounds().w(), 240);
}

TEST(AbsoluteMoveAndResize, LoadInvalidSmallNumber) {
  TEST_YAML(root, R"(
x: -1
y: 40
width: 320
height: 240
)");
  EXPECT_THROW(
    AbsoluteMoveAndResizeAction{root},
    winrt::hresult_invalid_argument);
}

TEST(AbsoluteMoveAndResize, LoadInvalidLargeNumber) {
  TEST_YAML(root, R"(
x: 40
y: 40
width: 16385
height: 240
)");
  EXPECT_THROW(
    AbsoluteMoveAndResizeAction{root},
    winrt::hresult_invalid_argument);
}


// ---[ RelativeMoveAndResize ]----------------------------

TEST(RelativeMoveAndResize, LoadValidYaml) {
  TEST_YAML(root, R"(
id: 3
x: 0
y: 1
width: 0.75
height: 0.9
)");
  RelativeMoveAndResizeAction const action{root};
  EXPECT_EQ(action.id(), 3);
  EXPECT_EQ(action.windowBounds().x(), 0.f);
  EXPECT_EQ(action.windowBounds().y(), 1.0f);
  EXPECT_EQ(action.windowBounds().z(), 0.75f);
  EXPECT_EQ(action.windowBounds().w(), 0.9f);
}

TEST(RelativeMoveAndResize, LoadValidYamlWithoutAll) {
  TEST_YAML(root, R"(
{}
)");
  RelativeMoveAndResizeAction const action{root};
  EXPECT_EQ(action.id(), 0);
  EXPECT_EQ(action.windowBounds().x(), 0.5f);
  EXPECT_EQ(action.windowBounds().y(), 0.5f);
  EXPECT_EQ(action.windowBounds().z(), 1.f);
  EXPECT_EQ(action.windowBounds().w(), 1.f);
}

TEST(RelativeMoveAndResize, LoadInvalidSmallNumber) {
  TEST_YAML(root, R"(
x: -0.001
y: 0.5
width: 0.75
height: 1
)");
  EXPECT_THROW(
    RelativeMoveAndResizeAction{root},
    winrt::hresult_invalid_argument);
}

TEST(RelativeMoveAndResize, LoadInvalidLargeNumber) {
  TEST_YAML(root, R"(
x: 0
y: 0.5
width: 0.75
height: 1.001
)");
  EXPECT_THROW(
    RelativeMoveAndResizeAction{root},
    winrt::hresult_invalid_argument);
}

}  // namespace test::roxyg::settings::action
