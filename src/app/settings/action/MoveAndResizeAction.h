#pragma once

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

  [[nodiscard]] constexpr numeric::float4 windowBounds() const noexcept {
    return window_bounds_;
  }

private:
  c4::yml::NodeRef node_;
  numeric::float4 window_bounds_;
};

}
