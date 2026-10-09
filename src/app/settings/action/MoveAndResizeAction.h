#pragma once
#include "../Action.h"
#include "../String.h"

#include "../../numeric/numeric.h"

namespace roxyg::settings::action {

class AbsoluteMoveAndResizeAction final {
public:
  explicit AbsoluteMoveAndResizeAction(c4::yml::NodeRef node);

  [[nodiscard]] constexpr numeric::long4 windowBounds() const noexcept {
    return window_bounds_;
  }

private:
  c4::yml::NodeRef node_;
  numeric::long4 window_bounds_;
};

class RelativeMoveAndResizeAction final {
public:
  explicit RelativeMoveAndResizeAction(c4::yml::NodeRef node);

  [[nodiscard]] constexpr int id() const noexcept {
    return id_;
  }

  [[nodiscard]] constexpr StringAndCompareType const& name() const noexcept {
    return name_;
  }

  [[nodiscard]] constexpr numeric::float4 windowBounds() const noexcept {
    return window_bounds_;
  }

private:
  c4::yml::NodeRef node_;
  settings::StringAndCompareType name_;
  numeric::float4 window_bounds_;
  int id_;
};

}

namespace roxyg::settings::detail {

template<>
struct ActionTypeTraits<action::AbsoluteMoveAndResizeAction> {
  static constexpr ActionType value = ActionType::kAbsoluteMoveAndResize;
};

template<>
struct ActionTypeTraits<action::RelativeMoveAndResizeAction> {
  static constexpr ActionType value = ActionType::kRelativeMoveAndResize;
};

}
