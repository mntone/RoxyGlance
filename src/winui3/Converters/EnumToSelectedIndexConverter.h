#pragma once
#include "Converters/EnumToSelectedIndexConverter.g.h"

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

struct EnumToSelectedIndexConverter: EnumToSelectedIndexConverterT<EnumToSelectedIndexConverter, non_agile, no_weak_ref> {
  EnumToSelectedIndexConverter() = default;

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
};

}

namespace winrt::Mntone::RoxyGlance::Converters::factory_implementation {

struct EnumToSelectedIndexConverter: EnumToSelectedIndexConverterT<EnumToSelectedIndexConverter, implementation::EnumToSelectedIndexConverter> {};

}
