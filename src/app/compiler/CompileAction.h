#pragma once

#include "../engine/Operation.h"
#include "../settings/Action.h"

namespace roxyg::compiler {

extern engine::OperationSet CompileAction(settings::Actions const& action);

}
