#pragma once

#include "../../engine/Predicate.h"
#include "../../settings/String.h"

namespace roxyg::compiler::filter {

engine::Predicate CompileProcessImageName(settings::StringAndMatchType const& conf);
engine::Predicate CompileWindowClass(settings::StringAndMatchType const& conf);
engine::Predicate CompileWindowTitle(settings::StringAndMatchType const& conf);

}
