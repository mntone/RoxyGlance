#include "pch.h"
#include "RuleEngine.h"

#include "../compiler/CompileRule.h"
#include "../context/OperationContext.h"
#include "../settings/UserSettings.h"
#include "../window/WindowStateCache.h"

using namespace roxyg;
using namespace roxyg::engine;

void RuleEngine::onSettingsChanged(settings::UserSettingsDocument const& settings) noexcept {
  rule_set_ = compiler::CompileRule(settings);
}

void RuleEngine::onForegroundEvent(window::State& windowState) noexcept {
  OperationContext* ctx = operation_context_;
  if (!ctx) {
    return;
  }

  for (auto& rule : rule_set_) {
    for (auto& [evaluate, data] : rule.condition) {
      if (!evaluate(data, windowState)) {
        return;
      }
    }
    for (auto& operation : rule.operations) {
      operation->execute(*ctx, windowState);
    }
  }
}

void RuleEngine::onShowEvent(window::State& windowState) noexcept {
  OperationContext* ctx = operation_context_;
  if (!ctx) {
    return;
  }

  for (auto& rule : rule_set_) {
    for (auto& [evaluate, data] : rule.condition) {
      if (!evaluate(data, windowState)) {
        return;
      }
    }
    for (auto& operation : rule.operations) {
      operation->execute(*ctx, windowState);
    }
  }
}
