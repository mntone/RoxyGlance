#pragma once
#include "Converters/ZeroToCollapsedConverter.g.h"

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

struct ZeroToCollapsedConverter: ZeroToCollapsedConverterT<ZeroToCollapsedConverter> {
  ZeroToCollapsedConverter() = default;

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

struct ZeroToCollapsedConverter: ZeroToCollapsedConverterT<ZeroToCollapsedConverter, implementation::ZeroToCollapsedConverter> {};

}
