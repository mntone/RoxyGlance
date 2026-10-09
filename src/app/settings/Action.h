#pragma once

namespace roxyg::settings {

enum class ActionType {
  kInvalid = 0,
  kAbsoluteMoveAndResize,
  kRelativeMoveAndResize,
};

namespace detail {

template<typename T>
struct ActionTypeTraits;

}

class Action final {
public:
  explicit Action(c4::yml::NodeRef node);

  [[nodiscard]] constexpr ActionType type() const noexcept {
    return type_;
  }
  void setType(ActionType value);

  template<typename T>
  [[nodiscard]] T as() const {
    if (type_ != detail::ActionTypeTraits<T>::value) {
      throw ::winrt::hresult_invalid_argument();
    }
    return T{node_};
  }

private:
  c4::yml::NodeRef node_;
  ActionType type_;
};

}

namespace magic_enum::customize {

template<>
constexpr customize_t enum_name(roxyg::settings::ActionType value) noexcept {
  using AT = roxyg::settings::ActionType;

  switch (value) {
  case AT::kAbsoluteMoveAndResize:
    return "absolute_move_and_resize";
  case AT::kRelativeMoveAndResize:
    return "relative_move_and_resize";
  default:
    return invalid_tag;
  }
}

}  // namespace magic_enum::customize
