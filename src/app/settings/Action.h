#pragma once

namespace roxyg::settings {

enum class ActionType {
  kUnknown = 0,
  kAbsoluteMoveAndResize,
  kRelativeMoveAndResize,
};

class Action final {
public:
  explicit Action(c4::yml::NodeRef node);

  [[nodiscard]] inline c4::yml::NodeRef node() const noexcept {
    return node_;
  }

  [[nodiscard]] constexpr ActionType type() const noexcept {
    return type_;
  }

private:
  c4::yml::NodeRef node_;
  ActionType type_;
};

using Actions = boost::container::small_vector<Action, 1>;

}
