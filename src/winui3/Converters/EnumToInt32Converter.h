#pragma once
#include "Converters/EnumToInt32Converter.g.h"

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

struct EnumToInt32Converter: EnumToInt32ConverterT<EnumToInt32Converter, non_agile, no_weak_ref> {
  EnumToInt32Converter() = default;

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

struct EnumToInt32Converter: EnumToInt32ConverterT<EnumToInt32Converter, implementation::EnumToInt32Converter> {};

}
