#pragma once
#include "pch.h"
#include "initialize.h"

#include "StringFilterViewModel.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

std::once_flag initialized;

void initialize() {
  std::call_once(initialized, [] {
    StringFilterViewModel::initialize();
  });
}

}
