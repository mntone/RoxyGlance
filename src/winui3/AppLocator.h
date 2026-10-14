#pragma once
#ifdef __INTELLISENSE__
#include "../app/utility/macro.h"
#endif

#include "App.xaml.h"

namespace roxyg::winui3 {

ROXYG_ALWAYS_INLINE roxyg::AppDelegate& GetAppDelegate() {
  ::winrt::com_ptr<::winrt::Mntone::RoxyGlance::implementation::App> app_self{
    ::winrt::Microsoft::UI::Xaml::Application::Current()
      .as<::winrt::Mntone::RoxyGlance::implementation::App>()
  };
  return app_self->AppDelegate();
}

}
