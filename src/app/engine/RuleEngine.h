#pragma once

#include "../context/OperationContext.h"
#include "../settings/ISettingsListener.h"
#include "../window/IWindowStateListener.h"

namespace roxyg::engine {

struct Rule;
using RuleSet = std::vector<Rule>;

class RuleEngine final
  : public settings::ISettingsListener
  , public window::IWindowStateListener {
  RuleEngine(RuleEngine const&) = delete;
  RuleEngine& operator=(RuleEngine const&) = delete;

public:
  constexpr RuleEngine() noexcept
    : operation_context_(nullptr) {
  }

  void onSettingsChanged(settings::UserSettingsDocument const& settings) noexcept override;
  void onForegroundEvent(window::State& state) noexcept override;
  void onShowEvent(window::State& state) noexcept override;

  constexpr OperationContext const* operationContext() const noexcept { return operation_context_; }
  constexpr void setOperationContext(OperationContext* ctx) noexcept { operation_context_ = ctx; }

private:
  void checkRules(window::State& windowState) noexcept;

private:
  OperationContext* operation_context_;
  RuleSet rule_set_;
};

}
