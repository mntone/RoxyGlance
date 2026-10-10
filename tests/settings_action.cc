#include "pch.h"
#include "settings_shared.h"
#include "app/settings/constants.h"
#include "app/settings/action/MoveAndResizeAction.h"

namespace test::roxyg::settings::action {

using namespace ::roxyg::settings;
using namespace ::roxyg::settings::action;
using namespace ::roxyg::settings::numeric_limit;

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

TEST(AbsoluteMoveAndResize, WriteBounds) {
  TEST_YAML(root, "x: 1\ny: 2\nwidth: 3\nheight: 4\nother: preserved");

  AbsoluteMoveAndResizeAction action{root};
  action.setX(30);
  action.setY(40);
  action.setWidth(800);
  action.setHeight(600);

  EXPECT_EQ(action.windowBounds().x(), 30);
  EXPECT_EQ(action.windowBounds().y(), 40);
  EXPECT_EQ(action.windowBounds().z(), 800);
  EXPECT_EQ(action.windowBounds().w(), 600);
  EXPECT_EQ(root["other"].val(), c4::to_csubstr("preserved"));

  AbsoluteMoveAndResizeAction const reloaded{root};
  EXPECT_EQ(reloaded.windowBounds().x(), 30);
  EXPECT_EQ(reloaded.windowBounds().y(), 40);
  EXPECT_EQ(reloaded.windowBounds().z(), 800);
  EXPECT_EQ(reloaded.windowBounds().w(), 600);
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

TEST(RelativeMoveAndResize, WriteProperties) {
  TEST_YAML(root, R"(
id: 2
name: ^Primary$
x: 0.1
y: 0.2
width: 0.3
height: 0.4
other: preserved
)");

  RelativeMoveAndResizeAction action{root};
  action.setId(4);
  action.setName({StringMatchType::kStartsWith, L"Secondary"});
  action.setX(0.5f);
  action.setY(0.6f);
  action.setWidth(0.7f);
  action.setHeight(0.8f);

  EXPECT_EQ(action.id(), 4);
  EXPECT_EQ(action.name(), (std::make_pair(StringMatchType::kStartsWith, L"Secondary")));
  EXPECT_FLOAT_EQ(action.windowBounds().x(), 0.5f);
  EXPECT_FLOAT_EQ(action.windowBounds().y(), 0.6f);
  EXPECT_FLOAT_EQ(action.windowBounds().z(), 0.7f);
  EXPECT_FLOAT_EQ(action.windowBounds().w(), 0.8f);
  EXPECT_EQ(root["other"].val(), c4::to_csubstr("preserved"));

  RelativeMoveAndResizeAction const reloaded{root};
  EXPECT_EQ(reloaded.id(), 4);
  EXPECT_EQ(reloaded.name(), (std::make_pair(StringMatchType::kStartsWith, L"Secondary")));
  EXPECT_FLOAT_EQ(reloaded.windowBounds().x(), 0.5f);
  EXPECT_FLOAT_EQ(reloaded.windowBounds().y(), 0.6f);
  EXPECT_FLOAT_EQ(reloaded.windowBounds().z(), 0.7f);
  EXPECT_FLOAT_EQ(reloaded.windowBounds().w(), 0.8f);
}

TEST(RelativeMoveAndResize, WriteUnsetIdRemovesNode) {
  TEST_YAML(root, "id: 4\nother: preserved");

  RelativeMoveAndResizeAction action{root};
  action.setId(kRelativeMonitorIdUnset);

  EXPECT_EQ(action.id(), kRelativeMonitorIdUnset);
  EXPECT_FALSE(root.has_child(key::kMonitorId));
  EXPECT_EQ(root["other"].val(), c4::to_csubstr("preserved"));

  RelativeMoveAndResizeAction const reloaded{root};
  EXPECT_EQ(reloaded.id(), kRelativeMonitorIdUnset);
}

}  // namespace test::roxyg::settings::action
