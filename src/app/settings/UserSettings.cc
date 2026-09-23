#include "pch.h"
#include "UserSettings.h"

#include "constants.h"

using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE std::vector<Rule> readRules(c4::yml::NodeRef n) {
  if (!n.has_child(key::kRules)) {
    return {};
  }

  c4::yml::NodeRef rules = n[key::kRules];
  if (!rules.is_seq()) {
    return {};
  }

  size_t const rules_count = rules.num_children();
  if (rules_count == 0) {
    return {};
  }

  std::vector<Rule> ret;
  ret.reserve(rules_count);
  for (c4::yml::NodeRef rule : rules.children()) {
    ret.emplace_back(rule);
  }
  return ret;
}

UserSettings::UserSettings(c4::yml::NodeRef node)
  : node_(std::move(node))
  , rules_(readRules(node_)) {
}

void UserSettingsDocument::load(std::string_view yaml) {
  c4::yml::parse_in_arena(c4::to_csubstr(yaml), &tree_);
  root_ = std::make_unique<UserSettings>(tree_.rootref());
}
