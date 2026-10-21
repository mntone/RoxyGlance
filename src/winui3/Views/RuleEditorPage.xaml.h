#pragma once
#include "Views/RuleEditorPage.g.h"

namespace roxyg::settings { class UserSettingsStore; }

namespace winrt::Mntone::RoxyGlance::Views::implementation {

struct RuleEditorPage: RuleEditorPageT<RuleEditorPage> {
  RuleEditorPage();
  RuleEditorPage(roxyg::settings::UserSettingsStore& store);

  ViewModels::Settings::RulesViewModel Rules() noexcept {
    return Rules_;
  }
  ViewModels::Shell::MonitorsViewModel Monitors() noexcept {
    return Monitors_;
  }

  void SaveClick(
    Windows::Foundation::IInspectable const& sender,
    Microsoft::UI::Xaml::RoutedEventArgs const& args
  ) const;

private:
  roxyg::settings::UserSettingsStore& store_;
  ViewModels::Settings::RulesViewModel Rules_;
  ViewModels::Shell::MonitorsViewModel Monitors_;
};

}

namespace winrt::Mntone::RoxyGlance::Views::factory_implementation {

struct RuleEditorPage: RuleEditorPageT<RuleEditorPage, implementation::RuleEditorPage> {};

}
