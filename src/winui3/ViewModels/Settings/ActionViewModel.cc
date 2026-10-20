#include "pch.h"
#include "ActionViewModel.h"
#if __has_include("ViewModels/Settings/ActionViewModel.g.cpp")
#include "ViewModels/Settings/ActionViewModel.g.cpp"
#endif

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

bool ActionViewModel::HasPendingChanges() const noexcept {
  return false;
}

bool ActionViewModel::applyChanges() {
  return false;
}

}
