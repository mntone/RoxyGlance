#pragma once
#include "CompilationContext.h"

#include "../engine/Operation.h"
#include "../settings/Action.h"

namespace roxyg::compiler {

engine::Operation CompileAction(CompilationContext& ctx, settings::Action const& action);

}
