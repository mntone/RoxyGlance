#include "pch.h"
#include "EnumToInt32Converter.h"
#if __has_include("Converters/EnumToInt32Converter.g.cpp")
#include "Converters/EnumToInt32Converter.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using namespace ::winrt::Microsoft::UI::Xaml;
}

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

IInspectable EnumToInt32Converter::Convert(
  IInspectable const& value,
  [[maybe_unused]] TypeName const& targetType,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
#ifdef _DEBUG
  WINRT_ASSERT(xaml_typename<int32_t>() == targetType);
#endif

  if (IPropertyValue property_value = value.try_as<IPropertyValue>()) {
    int32_t const raw_value = property_value.GetInt32();
    return box_value(raw_value);
  }

  return box_value(-1);
}

IInspectable EnumToInt32Converter::ConvertBack(
  IInspectable const& value,
  TypeName const& /*targetType*/,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
  int32_t const raw_value{unbox_value<int32_t>(value)};
  if (raw_value < 0) {
    throw hresult_invalid_argument();
  }

  return box_value(raw_value);
}

}  // namespace winrt::Mntone::RoxyGlance::Converters::implementation
