#pragma once
#include "TriggerFlags.h"
#include "Action.h"
#include "Filter.h"

namespace roxyg::settings {

class Rule final {
public:
  explicit Rule(c4::yml::NodeRef node);

  [[nodiscard]] constexpr std::wstring_view name() const noexcept { return name_; }

  [[nodiscard]] constexpr TriggerFlags triggerFlags() const noexcept { return triggers_; }
  void setTriggerFlags(TriggerFlags value);

  [[nodiscard]] constexpr Filter& filter() noexcept { return filter_; }
  [[nodiscard]] constexpr Filter const& filter() const noexcept { return filter_; }

  [[nodiscard]] constexpr Action& action() noexcept { return action_; }
  [[nodiscard]] constexpr Action const& action() const noexcept { return action_; }

private:
  c4::yml::NodeRef node_;
  std::wstring name_;
  Filter filter_;
  Action action_;
  TriggerFlags triggers_;
};

}  // namespace roxyg::settings
