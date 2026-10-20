#include "pch.h"
#include "SettingsViewModelProperties.h"

namespace winrt {
using namespace ::winrt::Microsoft::UI::Xaml::Data;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs SettingsViewModelProperties::HasPendingChangesPropertyChangedEventArgs_{nullptr};

void SettingsViewModelProperties::initialize() {
  HasPendingChangesPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"HasPendingChanges"};
}

}
