#pragma once
#include "ViewModels/Settings/ActionViewModel.g.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct ActionViewModel: ActionViewModelT<ActionViewModel, non_agile> {
  explicit inline ActionViewModel(ActionType type)
    : Type_(type) {
  }

  inline constexpr ActionType Type() const noexcept {
    return Type_;
  }

  virtual void save() {};

private:
  ActionType const Type_;
};

}

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::factory_implementation {

struct ActionViewModel: ActionViewModelT<ActionViewModel, implementation::ActionViewModel> {};

}
