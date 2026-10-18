#include "pch.h"
#include "MenuFlyoutHelper.h"
#if __has_include("Helpers/MenuFlyoutHelper.g.cpp")
#include "Helpers/MenuFlyoutHelper.g.cpp"
#endif

#include "../../app/winrt/weak_delegate.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Controls;
using namespace ::winrt::Microsoft::UI::Xaml::Interop;
using namespace ::winrt::Mntone::RoxyGlance::Helpers;
}

using namespace roxyg::winrt;

namespace winrt::Mntone::RoxyGlance::Helpers::implementation {

DependencyProperty MenuFlyoutHelper::ItemsSourceProperty_{
  DependencyProperty::RegisterAttached(
    L"ItemsSource",
    xaml_typename<IInspectable>(),
    xaml_typename<winrt::MenuFlyoutHelper>(),
    PropertyMetadata{nullptr, PropertyChangedCallback(&MenuFlyoutHelper::OnItemsSourceChanged)}
  )
};
DependencyProperty MenuFlyoutHelper::ItemsSourceRevokerProperty_{
  DependencyProperty::RegisterAttached(
    L"ItemsSourceRevoker",
    xaml_typename<IInspectable>(),
    xaml_typename<winrt::MenuFlyoutHelper>(),
    PropertyMetadata{nullptr}
  )
};
DependencyProperty MenuFlyoutHelper::ItemTemplateProperty_{
  DependencyProperty::RegisterAttached(
    L"ItemTemplate",
    xaml_typename<DataTemplate>(),
    xaml_typename<winrt::MenuFlyoutHelper>(),
    PropertyMetadata{nullptr, PropertyChangedCallback(&MenuFlyoutHelper::OnItemTemplateChanged)}
  )
};

struct VectorChangedEventBag: implements<VectorChangedEventBag, IInspectable, non_agile, no_weak_ref> {
  IObservableVector<IInspectable>::VectorChanged_revoker revoker;

  explicit inline VectorChangedEventBag(
    IObservableVector<IInspectable>::VectorChanged_revoker&& revoker
  ) noexcept: revoker(std::move(revoker)) {
  }
};

static ROXYG_ALWAYS_INLINE void ClearItems(MenuFlyout const& flyout) {
  flyout.SetValue(MenuFlyoutHelper::ItemsSourceRevokerProperty(), nullptr);
  flyout.Items().Clear();
}

void MenuFlyoutHelper::OnItemsSourceChanged(
  DependencyObject const& dp,
  DependencyPropertyChangedEventArgs const& args
) {
  MenuFlyout const flyout{dp.as<MenuFlyout>()};
  IObservableVector<IInspectable> observable{args.NewValue().try_as<IObservableVector<IInspectable>>()};
  if (!observable) {
    ClearItems(flyout);
    return;
  }

  auto revoker = observable.VectorChanged(auto_revoke, weak_delegate(flyout, [](
    MenuFlyout const& flyout,
    IObservableVector<IInspectable> const& sender,
    IVectorChangedEventArgs const&) {
    // TODO: differential update
    UpdateItems(flyout, sender, GetItemTemplate(flyout));
  }));
  flyout.SetValue(ItemsSourceRevokerProperty_, winrt::make<VectorChangedEventBag>(std::move(revoker)));

  UpdateItems(flyout, observable, GetItemTemplate(flyout));
}

void MenuFlyoutHelper::OnItemTemplateChanged(
  DependencyObject const& dp,
  DependencyPropertyChangedEventArgs const& args
) {
  MenuFlyout const flyout{dp.as<MenuFlyout>()};
  IVector<IInspectable> vec{GetItemsSource(flyout).as<IVector<IInspectable>>()};
  if (!vec) {
    ClearItems(flyout);
    return;
  }

  DataTemplate const temp{args.NewValue().as<DataTemplate>()};
  UpdateItems(flyout, vec, temp);
}

void MenuFlyoutHelper::UpdateItems(
  MenuFlyout const& flyout,
  IVector<IInspectable> const& vec,
  DataTemplate const& temp
) {
  std::vector<MenuFlyoutItemBase> new_items;
  new_items.reserve(vec.Size());
  if (temp) {
    for (IInspectable item : vec) {
      MenuFlyoutItemBase elem{temp.LoadContent().as<MenuFlyoutItemBase>()};
      elem.DataContext(item);
      new_items.emplace_back(elem);
    }
  } else {
    for (IInspectable item : vec) {
      MenuFlyoutItem elem;
      elem.Text(item.as<hstring>());
      new_items.emplace_back(elem);
    }
  }
  flyout.Items().ReplaceAll(std::move(new_items));
}

}  // namespace winrt::Mntone::RoxyGlance::Helpers::implementation
