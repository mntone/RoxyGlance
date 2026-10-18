#pragma once
#include "Converters/TypeNameTemplateSelector.g.h"

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

struct TypeNameTemplateSelector: TypeNameTemplateSelectorT<TypeNameTemplateSelector, no_weak_ref> {
  TypeNameTemplateSelector() = default;

  inline Microsoft::UI::Xaml::DataTemplate EmptyItemTemplate() const noexcept {
    return EmptyItemTemplate_;
  }
  inline void EmptyItemTemplate(Microsoft::UI::Xaml::DataTemplate const& value) noexcept {
    EmptyItemTemplate_ = value;
  }

  inline Microsoft::UI::Xaml::ResourceDictionary Resources() const noexcept {
    return Resources_;
  }
  inline void Resources(Microsoft::UI::Xaml::ResourceDictionary const& value) noexcept {
    Resources_ = value;
  }

  Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
    Windows::Foundation::IInspectable const& item
  ) const;
  Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(
    Windows::Foundation::IInspectable const& item,
    Microsoft::UI::Xaml::DependencyObject const& container
  ) const;

private:
  Microsoft::UI::Xaml::DataTemplate EmptyItemTemplate_;
  Microsoft::UI::Xaml::ResourceDictionary Resources_;
};

}

namespace winrt::Mntone::RoxyGlance::Converters::factory_implementation {

struct TypeNameTemplateSelector: TypeNameTemplateSelectorT<TypeNameTemplateSelector, implementation::TypeNameTemplateSelector> {};

}
