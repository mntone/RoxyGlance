#include "pch.h"
#include "RuleEditorControl.xaml.h"
#if __has_include("Controls/RuleEditorControl.g.cpp")
#include "Controls/RuleEditorControl.g.cpp"
#endif

#include "../Helpers/KeyboardModifiers.h"  // requires KeyboardModifiers

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::System;
using namespace ::winrt::Windows::UI::Core;

using namespace ::winrt::Microsoft::UI::Input;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Controls;
using namespace ::winrt::Microsoft::UI::Xaml::Input;
using namespace ::winrt::Microsoft::UI::Xaml::Media;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell;
}

namespace winrt::Mntone::RoxyGlance::Controls::implementation {

DependencyProperty RuleEditorControl::RuleProperty_{
  DependencyProperty::Register(
    L"Rule",
    xaml_typename<RuleViewModel>(),
    xaml_typename<Controls::RuleEditorControl>(),
    PropertyMetadata{nullptr}
  )
};
DependencyProperty RuleEditorControl::MonitorsProperty_{
  DependencyProperty::Register(
    L"Monitors",
    xaml_typename<MonitorsViewModel>(),
    xaml_typename<Controls::RuleEditorControl>(),
    PropertyMetadata{nullptr}
  )
};

static inline FrameworkElement FindDirectChildByTabIndex(DependencyObject parent, int32_t tabindex) {
  ROXYG_UNCHECKED_ASSERT(parent);

  int32_t const count = VisualTreeHelper::GetChildrenCount(parent);
  for (int32_t i = 0; i < count; ++i) {
    DependencyObject const child{VisualTreeHelper::GetChild(parent, i)};
    if (FrameworkElement const elem = child.try_as<FrameworkElement>()) {
      if (elem.TabIndex() == tabindex) {
        return elem;
      }
    }
  }
  return nullptr;
}

void RuleEditorControl::TriggersViewKeyDown(IInspectable const&, KeyRoutedEventArgs const& args) {
  roxyg::winui3::KeyboardModifiers const states;

  // Handle Tab when no modifier keys are pressed.
  if (args.Key() == VirtualKey::Tab && states.isNoModifierPressed()) {
    if (UIElement const row = FiltersView().TryGetElement(0)) {
      if (FrameworkElement const child = FindDirectChildByTabIndex(row, 1)) {
        args.Handled(child.Focus(FocusState::Keyboard));
      }
    }
    return;
  }
}

void RuleEditorControl::FilterRootKeyDown(IInspectable const& sender, KeyRoutedEventArgs const& args) {
  roxyg::winui3::KeyboardModifiers const states;
  VirtualKey const current_key{args.Key()};

  // Handle Escape when no modifier keys are pressed.
  if (current_key == VirtualKey::Escape && states.isNoModifierPressed()) {
    if (FrameworkElement const elem = sender.try_as<FrameworkElement>()) {
      ItemsRepeater const filters_view{FiltersView()};
      int32_t const index = filters_view.GetElementIndex(elem);
      if (UIElement const target = filters_view.TryGetElement(index)) {
        args.Handled(target.Focus(FocusState::Keyboard));
      }
    }
    return;
  }

  // Handle Shift+Tab on the Grid itself when no other modifiers are pressed.
  if (current_key == VirtualKey::Tab && states.isShiftOnlyPressed() && sender == args.OriginalSource()) {
    if (FrameworkElement const elem = sender.try_as<FrameworkElement>()) {
      ItemsRepeater const filters_view{FiltersView()};
      int32_t const index = filters_view.GetElementIndex(elem);
      if (index == 0) {
        args.Handled(AppLaunchCheckBox().Focus(FocusState::Keyboard));
      } else if (UIElement const previous_row = filters_view.TryGetElement(index - 1)) {
        args.Handled(previous_row.Focus(FocusState::Keyboard));
      }
    }
    return;
  }
}

void RuleEditorControl::FilterValueKeyDown(IInspectable const& sender, KeyRoutedEventArgs const& args) {
  FrameworkElement const elem{sender.try_as<FrameworkElement>()};
  if (!elem) {
    return;
  }

  roxyg::winui3::KeyboardModifiers const states;
  VirtualKey const current_key{args.Key()};

  // Handle Tab and Shift+Tab when no other modifiers are pressed.
  if (current_key == VirtualKey::Tab) {
    int32_t const tabindex = elem.TabIndex();

    if (tabindex == 1 && states.isShiftOnlyPressed()) {
      ItemsRepeater const filters_view{FiltersView()};
      int32_t const index = filters_view.GetElementIndex(elem.Parent().as<FrameworkElement>());
      if (UIElement const row = filters_view.TryGetElement(index - 1)) {
        if (FrameworkElement const child = FindDirectChildByTabIndex(row, 2)) {
          args.Handled(child.Focus(FocusState::Keyboard));
        }
      } else {
        args.Handled(AppLaunchCheckBox().Focus(FocusState::Keyboard));
      }
    } else if (tabindex == 2 && states.isNoModifierPressed()) {
      ItemsRepeater const filters_view{FiltersView()};
      int32_t const index = filters_view.GetElementIndex(elem.Parent().as<FrameworkElement>());
      if (UIElement const row = filters_view.TryGetElement(index + 1)) {
        if (FrameworkElement const child = FindDirectChildByTabIndex(row, 1)) {
          args.Handled(child.Focus(FocusState::Keyboard));
        }
      } else {
        args.Handled(AddFilterButton().Focus(FocusState::Keyboard));
      }
    }
    return;
  }
}

}
