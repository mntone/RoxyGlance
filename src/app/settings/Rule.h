#pragma once
#include "Action.h"
#include "Filter.h"

namespace roxyg::settings {

enum class TriggerType {
  kNone = 0,
  kApplicationInit = 1 << 0,
  kWindowForeground = 1 << 1,
  kWindowShow = 1 << 2,
};

class Rule final {
public:
  explicit Rule(c4::yml::NodeRef node) noexcept;

  [[nodiscard]] constexpr std::wstring_view name() const noexcept { return name_; }

  [[nodiscard]] constexpr TriggerType trigger() const noexcept { return trigger_; }

  [[nodiscard]] constexpr Filter& filter() noexcept { return filter_; }
  [[nodiscard]] constexpr Filter const& filter() const noexcept { return filter_; }

  [[nodiscard]] constexpr Action& action() noexcept { return action_; }
  [[nodiscard]] constexpr Action const& action() const noexcept { return action_; }

private:
  c4::yml::NodeRef node_;
  std::wstring name_;
  Filter filter_;
  Action action_;
  TriggerType trigger_;
};

}  // namespace roxyg::settings

namespace magic_enum::customize {

template<>
struct enum_range<roxyg::settings::TriggerType> {
  static constexpr bool is_flags = true;
};

}  // namespace magic_enum::customize
