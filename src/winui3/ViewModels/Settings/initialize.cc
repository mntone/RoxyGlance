#pragma once
#include "pch.h"
#include "initialize.h"

#include "SettingsViewModelProperties.h"
#include "MoveAndResizeViewModelProperties.h"
#include "RelativeMoveAndResizeViewModel.h"
#include "StringFilterViewModel.h"
#include "TriggersViewModel.h"
#include "RuleViewModel.h"
#include "RulesViewModel.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

std::once_flag initialized;

void initialize() {
  std::call_once(initialized, [] {
    MoveAndResizeViewModelProperties::initialize();
    RelativeMoveAndResizeViewModel::initialize();
    RulesViewModel::initialize();
    RuleViewModel::initialize();
    SettingsViewModelProperties::initialize();
    StringFilterViewModel::initialize();
    TriggersViewModel::initialize();
  });
}

}
