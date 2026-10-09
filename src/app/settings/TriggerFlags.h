#pragma once

namespace roxyg::settings {

enum class TriggerFlags: uint32_t {
  kNone = 0,
  kApplicationStart = 1 << 0,
  kWindowFocus = 1 << 1,
  kWindowShow = 1 << 2,
};

}

namespace magic_enum::customize {

template<>
constexpr customize_t enum_name(roxyg::settings::TriggerFlags value) noexcept {
  using TF = roxyg::settings::TriggerFlags;

  switch (value) {
  case TF::kApplicationStart:
    return "init";
  case TF::kWindowFocus:
    return "focus";
  case TF::kWindowShow:
    return "show";
  default:
    return invalid_tag;
  }
}

template<>
struct enum_range<roxyg::settings::TriggerFlags> {
  static constexpr bool is_flags = true;
  static constexpr int min = 0;
  static constexpr int max = 7;
};

}  // namespace magic_enum::customize
