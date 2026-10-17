#pragma once

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct MoveAndResizeViewModelProperties {
  static void initialize();

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs XPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs YPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs WidthPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs HeightPropertyChangedEventArgs_;
};

}
