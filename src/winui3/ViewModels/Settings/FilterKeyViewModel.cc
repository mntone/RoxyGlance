#include "pch.h"
#include "FilterKeyViewModel.h"
#if __has_include("ViewModels/Settings/FilterKeyViewModel.g.cpp")
#include "ViewModels/Settings/FilterKeyViewModel.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Microsoft::UI::Xaml::Input;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

FilterKeyViewModel::FilterKeyViewModel(FilterKey key, ICommand command)
  : AddCommand_(command)
  , Key_(key) {
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
