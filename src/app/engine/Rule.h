#pragma once
#include "Operation.h"
#include "Predicate.h"

namespace roxyg::engine {

struct Rule final {
  OperationSet operations;
  PredicateSet condition;
};

using RuleSet = std::vector<Rule>;

}
