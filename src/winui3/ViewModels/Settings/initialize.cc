#pragma once
#include "pch.h"
#include "initialize.h"

#include "MoveAndResizeViewModelProperties.h"
#include "RelativeMoveAndResizeViewModel.h"
#include "StringFilterViewModel.h"
#include "TriggersViewModel.h"
#include "RuleViewModel.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

std::once_flag initialized;

void initialize() {
  std::call_once(initialized, [] {
    MoveAndResizeViewModelProperties::initialize();
    RelativeMoveAndResizeViewModel::initialize();
    RuleViewModel::initialize();
    StringFilterViewModel::initialize();
    TriggersViewModel::initialize();
  });
}

}
