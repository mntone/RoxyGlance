#pragma once
#include "ViewModels/Settings/FilterKeyViewModel.g.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct FilterKeyViewModel: FilterKeyViewModelT<FilterKeyViewModel, non_agile> {
  FilterKeyViewModel(FilterKey key, Microsoft::UI::Xaml::Input::ICommand command);

  inline Microsoft::UI::Xaml::Input::ICommand AddCommand() const noexcept {
    return AddCommand_;
  }
  inline constexpr FilterKey Key() const noexcept {
    return Key_;
  }

private:
  Microsoft::UI::Xaml::Input::ICommand const AddCommand_;
  FilterKey const Key_;
};

}
