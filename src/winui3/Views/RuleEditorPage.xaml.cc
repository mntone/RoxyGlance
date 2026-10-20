#include "pch.h"
#include "RuleEditorPage.xaml.h"
#if __has_include("Views/RuleEditorPage.g.cpp")
#include "Views/RuleEditorPage.g.cpp"
#endif

#include "../AppLocator.h"
#include "ViewModels/Settings/RulesViewModel.h"
#include "ViewModels/Shell/MonitorsViewModel.h"

#include "../../app/monitor/MonitorStateCache.h"
#include "../../app/settings/UserSettings.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Controls;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell;
}

using namespace roxyg::winui3;

namespace winrt::Mntone::RoxyGlance::Views::implementation {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation;

RuleEditorPage::RuleEditorPage(roxyg::settings::UserSettingsStore& store)
  : store_(store)
  , Rules_(make<RulesViewModel>(store_.document().root()->rules()))
  , Monitors_(make<MonitorsViewModel>(GetAppDelegate().monitorStateCache())) {
}

RuleEditorPage::RuleEditorPage()
  : RuleEditorPage(GetAppDelegate().settingsStore()) {
}

void RuleEditorPage::SaveClick(IInspectable const& /*sender*/, RoutedEventArgs const& /*args*/) const {
  if (get_self<RulesViewModel>(Rules_)->applyChanges()) {
    store_.save();
  }

#ifdef _DEBUG
  ContentDialog dialog;
  dialog.XamlRoot(XamlRoot());
  dialog.CloseButtonText(L"Close");
  dialog.Content(box_value(to_hstring(store_.document().str().c_str())));
  dialog.Title(box_value(L"[Debug] yaml output"));
  dialog.ShowAsync();
#endif
}

}  // namespace winrt::Mntone::RoxyGlance::Views::implementation
