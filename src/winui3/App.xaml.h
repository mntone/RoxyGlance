#pragma once
#include "App.xaml.g.h"

#include <winrt/Mntone.RoxyGlance.Views.h>

#include "../app/AppDelegate.h"
#include "../app/message/IMessageListener.h"

namespace winrt::Mntone::RoxyGlance::implementation {

struct App: AppT<App>, roxyg::message::IMessageListener {
  App();
  ~App() noexcept;

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

  constexpr roxyg::AppDelegate& AppDelegate() noexcept { return AppDelegate_; }
  constexpr roxyg::AppDelegate const& AppDelegate() const noexcept { return AppDelegate_; }

private:
  void exitApp();
  void showSettingsWindow(param::hstring page_type);

  void onAppExitRequested() noexcept override;
  void onShowSettingsRequested() noexcept override;

private:
  roxyg::AppDelegate AppDelegate_;
  Microsoft::UI::Dispatching::DispatcherQueue dispatcher_;

  Views::SettingsWindow settings_window_;
  event_token settings_window_closed_token_;
};

}
