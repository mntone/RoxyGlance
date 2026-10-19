#include "pch.h"
#include "RuleEngine.h"

#include "../compiler/CompileRule.h"
#include "../settings/UserSettings.h"
#include "../window/WindowStateCache.h"

#include <magic_enum/magic_enum_flags.hpp>  // requires enum_flags_test

namespace {

inline constexpr std::wstring_view kRuleCompilationOutOfMemory
  = L"Failed to allocate memory while updating the rule set.";

}

using namespace roxyg;
using namespace roxyg::engine;

void RuleEngine::checkRules(window::State& windowState, TriggerFlags trigger) noexcept {
  OperationContext* ctx = operation_context_;
  if (!ctx) {
    return;
  }

  std::shared_ptr<RuleSet const> rule_set{rule_set_.load(std::memory_order_acquire)};

  bool matched;
  for (Rule const& rule : *rule_set.get()) {
    if (!magic_enum::enum_flags_test(rule.trigger, trigger)) {
      continue;
    }

    matched = true;
    for (auto& [evaluate, data] : rule.condition) {
      if (!evaluate(data, windowState)) {
        matched = false;
        break;
      }
    }
    if (matched) {
      rule.operation->execute(*ctx, windowState);
    }
  }
}

void RuleEngine::onSettingsChanged(settings::UserSettingsDocument const& settings) noexcept {
  OperationContext* operationContext{operation_context_};
  if (!operationContext) {
    return;
  }

  compiler::CompilationContext ctx{operationContext->monitor()};
  try {
    RuleSet rule_set{compiler::CompileRule(ctx, settings)};
    rule_set_.store(std::make_shared<RuleSet const>(std::move(rule_set)), std::memory_order_release);
  } catch (std::bad_alloc const&) {
    logger_.error(winrt::hstring{kRuleCompilationOutOfMemory}, E_OUTOFMEMORY);
  }
}

void RuleEngine::onForegroundEvent(window::State& windowState) noexcept {
  checkRules(windowState, TriggerFlags::kWindowFocus);
}

void RuleEngine::onShowEvent(window::State& windowState) noexcept {
  checkRules(windowState, TriggerFlags::kWindowShow);
}
