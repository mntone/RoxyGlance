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
  void setX(long val);
  void setY(long val);
  void setWidth(long val);
  void setHeight(long val);

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
  void setId(int val);

  [[nodiscard]] constexpr StringAndMatchType const& name() const noexcept {
    return name_;
  }
  void setName(StringAndMatchType val);

  [[nodiscard]] constexpr numeric::float4 windowBounds() const noexcept {
    return window_bounds_;
  }
  void setX(float val);
  void setY(float val);
  void setWidth(float val);
  void setHeight(float val);

private:
  c4::yml::NodeRef node_;
  StringAndMatchType name_;
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
