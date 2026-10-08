#include "pch.h"
#include "CompileRule.h"

#include "CompileAction.h"
#include "filter/CompileFilter.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

RuleSet compiler::CompileRule(CompilationContext& ctx, settings::UserSettingsDocument const& settings) {
  settings::UserSettings const* root = settings.root();
  if (!root) {
    return {};
  }

  RuleSet rules;
  for (auto const& rule : root->rules()) {
    Operation operation{CompileAction(ctx, rule.action())};
    if (!operation) {
      continue;
    }

    PredicateSet predicates{filter::CompileFilter(rule.filter())};
    rules.emplace_back(std::move(operation), std::move(predicates));
  }
  return std::move(rules);
}
