#include "pch.h"
#include "CompileRule.h"

#include "CompileAction.h"
#include "filter/CompileFilter.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

RuleSet compiler::CompileRule(settings::UserSettingsDocument const& settings) {
  settings::UserSettings const* root = settings.root();
  if (!root) {
    return {};
  }

  RuleSet rules;
  for (auto const& rule : root->rules()) {
    OperationSet operations{CompileAction(rule.actions())};
    if (operations.empty()) {
      break;
    }

    PredicateSet predicates{filter::CompileFilter(rule.filter())};
    rules.emplace_back(std::move(operations), std::move(predicates));
  }
  return std::move(rules);
}
