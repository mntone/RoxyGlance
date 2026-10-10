#pragma once
#include "../Action.h"
#include "../String.h"

#include "../../numeric/numeric.h"

namespace roxyg::settings::action {

class AbsoluteMoveAndResize final {
public:
  explicit AbsoluteMoveAndResize(c4::yml::NodeRef node);

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

class RelativeMoveAndResize final {
public:
  explicit RelativeMoveAndResize(c4::yml::NodeRef node);

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
struct ActionTypeTraits<action::AbsoluteMoveAndResize> {
  static constexpr ActionType value = ActionType::kAbsoluteMoveAndResize;
};

template<>
struct ActionTypeTraits<action::RelativeMoveAndResize> {
  static constexpr ActionType value = ActionType::kRelativeMoveAndResize;
};

}
