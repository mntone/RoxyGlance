#include "pch.h"
#include "TypeNameTemplateSelector.h"
#if __has_include("Converters/TypeNameTemplateSelector.g.cpp")
#include "Converters/TypeNameTemplateSelector.g.cpp"
#endif

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::UI::Xaml::Interop;

using namespace ::winrt::Microsoft::UI::Xaml;
}

namespace winrt::Mntone::RoxyGlance::Converters::implementation {

DataTemplate TypeNameTemplateSelector::SelectTemplateCore(IInspectable const& item) const {
  if (!item) {
    return EmptyItemTemplate_;
  }

  hstring const item_typename{get_class_name(item)};
  DataTemplate const item_template{Resources_.Lookup(box_value(item_typename)).as<DataTemplate>()};
  ROXYG_UNCHECKED_ASSERT(item_template);
  return item_template;
}

DataTemplate TypeNameTemplateSelector::SelectTemplateCore(
  IInspectable const& item,
  DependencyObject const& /*container*/
) const {
  return SelectTemplateCore(item);
}

}
