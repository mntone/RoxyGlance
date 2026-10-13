#pragma once
#include "Helpers/MenuFlyoutHelper.g.h"

namespace winrt::Mntone::RoxyGlance::Helpers::implementation {

struct MenuFlyoutHelper final {
  static inline Microsoft::UI::Xaml::DependencyProperty ItemsSourceProperty() noexcept {
    return ItemsSourceProperty_;
  }
  static inline Microsoft::UI::Xaml::DependencyProperty ItemsSourceRevokerProperty() noexcept {
    return ItemsSourceRevokerProperty_;
  }
  static inline Microsoft::UI::Xaml::DependencyProperty ItemTemplateProperty() noexcept {
    return ItemTemplateProperty_;
  }

  static inline Windows::Foundation::IInspectable GetItemsSource(Microsoft::UI::Xaml::Controls::MenuFlyout const& target) {
    return target.GetValue(ItemsSourceProperty_)
      .as<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>>();
  }
  static inline void SetItemsSource(
    Microsoft::UI::Xaml::Controls::MenuFlyout const& target,
    Windows::Foundation::IInspectable const& value
  ) {
    target.SetValue(ItemsSourceProperty_, value);
  }

  static inline Microsoft::UI::Xaml::DataTemplate GetItemTemplate(Microsoft::UI::Xaml::Controls::MenuFlyout const& target) {
    return target.GetValue(ItemTemplateProperty_).as<Microsoft::UI::Xaml::DataTemplate>();
  }
  static inline void SetItemTemplate(
    Microsoft::UI::Xaml::Controls::MenuFlyout const& target,
    Microsoft::UI::Xaml::DataTemplate const& value
  ) {
    target.SetValue(ItemTemplateProperty_, value);
  }

private:
  static void OnItemsSourceChanged(
    Microsoft::UI::Xaml::DependencyObject const& sender,
    Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& args
  );
  static void OnItemTemplateChanged(
    Microsoft::UI::Xaml::DependencyObject const& sender,
    Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& args
  );
  static void UpdateItems(
    Microsoft::UI::Xaml::Controls::MenuFlyout const& target,
    Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> const& vec,
    Microsoft::UI::Xaml::DataTemplate const& temp
  );

private:
  static Microsoft::UI::Xaml::DependencyProperty ItemsSourceProperty_;
  static Microsoft::UI::Xaml::DependencyProperty ItemsSourceRevokerProperty_;
  static Microsoft::UI::Xaml::DependencyProperty ItemTemplateProperty_;
};

}

namespace winrt::Mntone::RoxyGlance::Helpers::factory_implementation {

struct MenuFlyoutHelper: MenuFlyoutHelperT<MenuFlyoutHelper, implementation::MenuFlyoutHelper> {};

}
