#include "pch.h"
#include "RuleEngine.h"

#include "../compiler/CompileRule.h"
#include "../context/OperationContext.h"
#include "../settings/UserSettings.h"
#include "../window/WindowStateCache.h"

using namespace roxyg;
using namespace roxyg::engine;

void RuleEngine::checkRules(window::State& windowState) noexcept {
  OperationContext* ctx = operation_context_;
  if (!ctx) {
    return;
  }

  bool matched;
  for (auto& rule : rule_set_) {
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
  rule_set_ = compiler::CompileRule(settings);
}

void RuleEngine::onForegroundEvent(window::State& windowState) noexcept {
  checkRules(windowState);
}

void RuleEngine::onShowEvent(window::State& windowState) noexcept {
  checkRules(windowState);
}
