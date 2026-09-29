#pragma once

#include "../../engine/Predicate.h"
#include "../../settings/String.h"

namespace roxyg::compiler::filter {

engine::Predicate CompileWindowClass(settings::StringAndCompareType const& conf);
engine::Predicate CompileWindowTitle(settings::StringAndCompareType const& conf);

}
