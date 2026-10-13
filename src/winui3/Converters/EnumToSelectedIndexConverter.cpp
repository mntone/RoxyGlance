#include "pch.h"
#include "EnumToSelectedIndexConverter.h"
#if __has_include("Converters/EnumToSelectedIndexConverter.g.cpp")
#include "Converters/EnumToSelectedIndexConverter.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using namespace ::winrt::Microsoft::UI::Xaml;
}

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

IInspectable EnumToSelectedIndexConverter::Convert(
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
    return box_value(raw_value - 1);
  }

  return box_value(-1);
}

IInspectable EnumToSelectedIndexConverter::ConvertBack(
  IInspectable const& value,
  TypeName const& /*targetType*/,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
  int32_t const raw_value{unbox_value<int32_t>(value) + 1};
  if (raw_value < 0) {
    throw hresult_invalid_argument();
  }

  return box_value(raw_value);
}

}  // namespace winrt::Mntone::RoxyGlance::Converters::implementation
