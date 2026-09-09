#pragma once

#include "../engine/Rule.h"
#include "../settings/UserSettings.h"

namespace roxyg::compiler {

engine::RuleSet CompileRule(settings::UserSettingsDocument const& settings);

}
