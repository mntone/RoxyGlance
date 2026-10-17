#pragma once
#include "Converters/FilterKeyToLocalizedStringConverter.g.h"

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

struct FilterKeyToLocalizedStringConverter
  : FilterKeyToLocalizedStringConverterT<FilterKeyToLocalizedStringConverter, non_agile, no_weak_ref> {
  FilterKeyToLocalizedStringConverter();

  Windows::Foundation::IInspectable Convert(
    Windows::Foundation::IInspectable const& value,
    Windows::UI::Xaml::Interop::TypeName const& targetType,
    Windows::Foundation::IInspectable const& parameter,
    hstring const& language
  ) const;
  Windows::Foundation::IInspectable ConvertBack(
    Windows::Foundation::IInspectable const& value,
    Windows::UI::Xaml::Interop::TypeName const& targetType,
    Windows::Foundation::IInspectable const& parameter,
    hstring const& language
  ) const;

private:
  Microsoft::Windows::ApplicationModel::Resources::ResourceLoader const loader_;
};

}

namespace winrt::Mntone::RoxyGlance::Converters::factory_implementation {

struct FilterKeyToLocalizedStringConverter: FilterKeyToLocalizedStringConverterT<FilterKeyToLocalizedStringConverter, implementation::FilterKeyToLocalizedStringConverter> {};

}
