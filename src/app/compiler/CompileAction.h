#pragma once
#include "CompilationContext.h"

#include "../engine/Operation.h"
#include "../settings/Action.h"

namespace roxyg::compiler {

extern engine::OperationSet CompileAction(CompilationContext& ctx, settings::Actions const& action);

}
