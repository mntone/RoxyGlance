#include "pch.h"
#include "RuleEngine.h"

#include "../compiler/CompileRule.h"
#include "../context/OperationContext.h"
#include "../settings/UserSettings.h"
#include "../window/WindowStateCache.h"

namespace {

inline constexpr std::wstring_view kRuleCompilationOutOfMemory
  = L"Failed to allocate memory while updating the rule set.";

}

using namespace roxyg;
using namespace roxyg::engine;

void RuleEngine::checkRules(window::State& windowState) noexcept {
  OperationContext* ctx = operation_context_;
  if (!ctx) {
    return;
  }

  std::shared_ptr<RuleSet const> rule_set{rule_set_.load(std::memory_order_acquire)};

  bool matched;
  for (auto& rule : *rule_set.get()) {
    matched = true;
    for (auto& [evaluate, data] : rule.condition) {
      if (!evaluate(data, windowState)) {
        matched = false;
        break;
      }
    }
    if (matched) {
      for (auto& operation : rule.operations) {
        operation->execute(*ctx, windowState);
      }
    }
  }
}

void RuleEngine::onSettingsChanged(settings::UserSettingsDocument const& settings) noexcept {
  try {
    RuleSet rule_set{compiler::CompileRule(settings)};
    rule_set_.store(std::make_shared<RuleSet const>(std::move(rule_set)), std::memory_order_release);
  } catch (std::bad_alloc const&) {
    logger_.error(winrt::hstring{kRuleCompilationOutOfMemory}, E_OUTOFMEMORY);
  }
}

void RuleEngine::onForegroundEvent(window::State& windowState) noexcept {
  checkRules(windowState);
}

void RuleEngine::onShowEvent(window::State& windowState) noexcept {
  checkRules(windowState);
}
