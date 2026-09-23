#include "pch.h"
#include "Rule.h"

namespace magic_enum::customize {

template<>
constexpr customize_t enum_name(roxyg::settings::TriggerType value) noexcept {
  using TT = roxyg::settings::TriggerType;

  switch (value) {
  case TT::kApplicationInit:
    return "init";
  case TT::kWindowForeground:
    return "foreground";
  case TT::kWindowShow:
    return "show";
  default:
    return invalid_tag;
  }
}

}  // namespace magic_enum::customize

#include "constants.h"
#include "NodeHelper.h"

using namespace magic_enum;
using namespace magic_enum::bitwise_operators;
using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE TriggerType loadTrigger(c4::yml::ConstNodeRef n) noexcept {
  std::string const trigger_type = ReadStringFromNode(n);
  return enum_cast<TriggerType>(trigger_type, case_insensitive)
    .value_or(TriggerType::kNone);
}

static ROXYG_ALWAYS_INLINE TriggerType readTriggers(c4::yml::ConstNodeRef triggers) noexcept {
  if (triggers.invalid()) {
    return {};
  }

  TriggerType ret = TriggerType::kApplicationInit | TriggerType::kWindowShow;
  if (triggers.is_seq()) {
    ret = TriggerType::kNone;
    for (c4::yml::ConstNodeRef trigger : triggers.children()) {
      ret |= loadTrigger(trigger);
    }
  } else {
    ret = loadTrigger(triggers);
  }
  return ret;
}

static ROXYG_ALWAYS_INLINE Filter readFilter(c4::yml::NodeRef n) {
  if (!n.has_child(key::kWhere)) {
    c4::yml::NodeRef target;
    if (n.has_child(key::kWhen)) {
      target = n.insert_child(n[key::kWhen]);
    } else if (n.has_child(key::kThen)) {
      target = n.insert_child(n[key::kThen].prev_sibling());
    } else {
      target = n.append_child();
    }

    c4::yml::NodeRef where = target << c4::yml::key(key::kWhere) << c4::yml::MAP;
    return Filter{ where };
  }

  return Filter{ n[key::kWhere] };
}

static ROXYG_ALWAYS_INLINE Actions readActions(c4::yml::NodeRef actions) {
  if (actions.invalid()) {
    return {};
  }

  Actions ret;
  if (actions.is_seq()) {
    size_t const actions_count = actions.num_children();
    if (actions_count != 0) {
      ret.reserve(actions_count);
      for (c4::yml::NodeRef action : actions.children()) {
        ret.emplace_back(action);
      }
    }
  } else {
    ret.emplace_back(actions);
  }
  return ret;
}

Rule::Rule(c4::yml::NodeRef node) noexcept
  : node_(std::move(node))
  , name_(ReadStringAsUtf16FromNode(node_[key::kName]))
  , filter_(readFilter(node_))
  , actions_(readActions(node_[key::kThen]))
  , trigger_(readTriggers(node_[key::kWhen])) {
}
