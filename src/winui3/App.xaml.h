#pragma once
#include "App.xaml.g.h"

#include <winrt/Mntone.RoxyGlance.Views.h>

#include "../app/AppDelegate.h"

namespace winrt::Mntone::RoxyGlance::implementation {

struct App: AppT<App>, roxyg::message::IMessageListener {
  App();
  ~App() noexcept;

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

  constexpr roxyg::AppDelegate& AppDelegate() noexcept { return AppDelegate_; }
  constexpr roxyg::AppDelegate const& AppDelegate() const noexcept { return AppDelegate_; }

private:
  void exitApp();
  void showMainWindow();

  void onAppExitRequested() noexcept override;
  void onShowSettingsRequested() noexcept override;

private:
  roxyg::AppDelegate AppDelegate_;
  Microsoft::UI::Dispatching::DispatcherQueue dispatcher_;

  Views::MainWindow main_window_;
  event_token main_window_closed_token_;
};

}
