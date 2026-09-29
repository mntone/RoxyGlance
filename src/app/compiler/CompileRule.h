#pragma once
#include "CompilationContext.h"

#include "../engine/Rule.h"
#include "../settings/UserSettings.h"

namespace roxyg::compiler {

engine::RuleSet CompileRule(CompilationContext& ctx, settings::UserSettingsDocument const& settings);

}
