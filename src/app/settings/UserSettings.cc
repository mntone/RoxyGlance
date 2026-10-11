#include "pch.h"
#include "UserSettings.h"
#include "ParseErrorPrivate.h"

#include "constants.h"
#include "NodeHelper.h"
#include "../logging/LogHelper.h"

using namespace c4::yml;
using namespace roxyg::logging;
using namespace roxyg::settings;
using namespace roxyg::settings::detail;

static ROXYG_ALWAYS_INLINE std::vector<Rule> readRules(NodeRef n, Logger* logger) {
  LogHelper<LogGroup::kSettings> log(logger);

  if (!n.has_child(key::kRules)) {
    return {};
  }

  NodeRef rules = n[key::kRules];
  if (!rules.is_seq()) {
    return {};
  }

  size_t const rules_count = rules.num_children();
  if (rules_count == 0) {
    return {};
  }

  std::vector<Rule> ret;
  ret.reserve(rules_count);

  size_t index = 1;
  for (NodeRef rule : rules.children()) {
    try {
      ret.emplace_back(rule);
    } catch (ParseError const& err) {
      std::wstring const rule_name{makeRuleDisplayName(rule, err, index)};
      winrt::hstring const content{makeParseErrorMessage(err, rule_name)};
      log.warn(content, E_INVALIDARG);
    }
    ++index;
  }
  return ret;
}

UserSettings::UserSettings(NodeRef node, logging::Logger* logger)
  : node_(std::move(node))
  , rules_(readRules(node_, logger)) {
}

void UserSettingsDocument::load(std::string_view yaml, logging::Logger* logger) {
  parse_in_arena(c4::to_csubstr(yaml), &tree_);
  root_ = std::make_unique<UserSettings>(tree_.rootref(), logger);
}
