#include "pch.h"
#include "FilterKeyToLocalizedStringConverter.h"
#if __has_include("Converters/FilterKeyToLocalizedStringConverter.g.cpp")
#include "Converters/FilterKeyToLocalizedStringConverter.g.cpp"
#endif

namespace {
inline constexpr std::wstring_view kProcessName{L"Process Name"};
inline constexpr std::wstring_view kWindowClass{L"Window Class"};
inline constexpr std::wstring_view kWindowTitle{L"Window Title"};
}

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Windows::UI::Xaml::Interop;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::Windows::ApplicationModel::Resources;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

FilterKeyToLocalizedStringConverter::FilterKeyToLocalizedStringConverter()
  : loader_() {
}

IInspectable FilterKeyToLocalizedStringConverter::Convert(
  IInspectable const& value,
  [[maybe_unused]] TypeName const& targetType,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
#ifdef _DEBUG
  WINRT_ASSERT(xaml_typename<hstring>() == targetType);
#endif

  FilterKey key{unbox_value<FilterKey>(value)};

  hstring localized_key;
  switch (key) {
  case FilterKey::ProcessName:
    localized_key = loader_.GetString(kProcessName);
    break;
  case FilterKey::WindowClass:
    localized_key = loader_.GetString(kWindowClass);
    break;
  case FilterKey::WindowTitle:
    localized_key = loader_.GetString(kWindowTitle);
    break;
  default:
    return box_value(L"");
  }
  return box_value(localized_key);
}

IInspectable FilterKeyToLocalizedStringConverter::ConvertBack(
  IInspectable const& /*value*/,
  TypeName const& /*targetType*/,
  IInspectable const& /*parameter*/,
  hstring const& /*language*/
) const {
  throw hresult_not_implemented();
}

}  // namespace winrt::Mntone::RoxyGlance::Converters::implementation
