#pragma once
#include "Controls/RuleEditorControl.g.h"

#include "../ViewModels/Settings/RuleViewModel.h"
#include "../ViewModels/Shell/MonitorsViewModel.h"

namespace winrt::Mntone::RoxyGlance::Controls::implementation {

struct RuleEditorControl: RuleEditorControlT<RuleEditorControl> {
  RuleEditorControl() = default;

  inline ViewModels::Settings::RuleViewModel Rule() const {
    return GetValue(RuleProperty_).as<ViewModels::Settings::RuleViewModel>();
  }

  inline void Rule(ViewModels::Settings::RuleViewModel const& value) {
    SetValue(RuleProperty_, value);
  }

  inline ViewModels::Shell::MonitorsViewModel Monitors() const {
    return GetValue(MonitorsProperty_).as<ViewModels::Shell::MonitorsViewModel>();
  }

  inline void Monitors(ViewModels::Shell::MonitorsViewModel const& value) {
    SetValue(MonitorsProperty_, value);
  }

  void TriggersViewKeyDown(
    winrt::Windows::Foundation::IInspectable const& sender,
    winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
  );
  void FilterRootKeyDown(
    winrt::Windows::Foundation::IInspectable const& sender,
    winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
  );
  void FilterValueKeyDown(
    winrt::Windows::Foundation::IInspectable const& sender,
    winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args
  );

  static inline Microsoft::UI::Xaml::DependencyProperty RuleProperty() noexcept {
    return RuleProperty_;
  }

  static inline Microsoft::UI::Xaml::DependencyProperty MonitorsProperty() noexcept {
    return MonitorsProperty_;
  }

  static inline ViewModels::Settings::RuleViewModel GetRule(Controls::RuleEditorControl const& target) {
    return target.GetValue(RuleProperty_).as<ViewModels::Settings::RuleViewModel>();
  }

  static inline void SetRule(Controls::RuleEditorControl const& target, ViewModels::Settings::RuleViewModel const& value) {
    target.SetValue(RuleProperty_, value);
  }

  static inline ViewModels::Shell::MonitorsViewModel GetMonitors(Controls::RuleEditorControl const& target) {
    return target.GetValue(MonitorsProperty_).as<ViewModels::Shell::MonitorsViewModel>();
  }

  static inline void SetMonitors(Controls::RuleEditorControl const& target, ViewModels::Shell::MonitorsViewModel const& value) {
    target.SetValue(MonitorsProperty_, value);
  }

private:
  static Microsoft::UI::Xaml::DependencyProperty RuleProperty_;
  static Microsoft::UI::Xaml::DependencyProperty MonitorsProperty_;
};

}

namespace winrt::Mntone::RoxyGlance::Controls::factory_implementation {

struct RuleEditorControl: RuleEditorControlT<RuleEditorControl, implementation::RuleEditorControl> {};

}
