#pragma once
#include "Operation.h"
#include "Predicate.h"

#include "../settings/TriggerFlags.h"

namespace roxyg::engine {

using TriggerFlags = settings::TriggerFlags;

struct Rule final {
  TriggerFlags trigger;
  Operation operation;
  PredicateSet condition;
};

using RuleSet = std::vector<Rule>;

}
