#include "pch.h"
#include "ZeroToCollapsedConverter.h"
#if __has_include("Converters/ZeroToCollapsedConverter.g.cpp")
#include "Converters/ZeroToCollapsedConverter.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using namespace ::winrt::Microsoft::UI::Xaml;
}

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

IInspectable ZeroToCollapsedConverter::Convert(
  IInspectable const& value,
  [[maybe_unused]] TypeName const& targetType,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
#ifdef _DEBUG
  WINRT_ASSERT(xaml_typename<Visibility>() == targetType);
#endif

  uint32_t size;
  if (IPropertyValue property_value = value.try_as<IPropertyValue>()) {
    size = property_value.GetUInt32();
  } else {
    size = 0;
  }

  bool const visible{size > 0};
  return box_value(visible ? Visibility::Visible : Visibility::Collapsed);
}

IInspectable ZeroToCollapsedConverter::ConvertBack(
  IInspectable const& /*value*/,
  TypeName const& /*targetType*/,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
  throw hresult_not_implemented();
}

}  // namespace winrt::Mntone::RoxyGlance::Converters::implementation
