#pragma once

#include "../../engine/Predicate.h"
#include "../../settings/Filter.h"

namespace roxyg::compiler::filter {

extern engine::PredicateSet CompileFilter(settings::Filter const& filter);

}
