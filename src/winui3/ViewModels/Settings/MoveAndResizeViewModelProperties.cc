#include "pch.h"
#include "MoveAndResizeViewModelProperties.h"

namespace winrt {
using namespace ::winrt::Microsoft::UI::Xaml::Data;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs MoveAndResizeViewModelProperties::XPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs MoveAndResizeViewModelProperties::YPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs MoveAndResizeViewModelProperties::WidthPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs MoveAndResizeViewModelProperties::HeightPropertyChangedEventArgs_{nullptr};

void MoveAndResizeViewModelProperties::initialize() {
  XPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"X"};
  YPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Y"};
  WidthPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Width"};
  HeightPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Height"};
}

}
