#pragma once

#include "../../engine/Predicate.h"
#include "../../settings/String.h"

namespace roxyg::compiler::filter {

extern engine::Predicate CompileWindowClass(settings::StringAndCompareType const& conf);
extern engine::Predicate CompileWindowTitle(settings::StringAndCompareType const& conf);

}
