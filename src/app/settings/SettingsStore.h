#pragma once
#include "ISettingsListener.h"
#include "UserSettings.h"

#include "../utility/ListenerHost.h"

namespace roxyg::settings {

class SettingsStore final
  : public utility::ListenerHost<ISettingsListener> {
public:
  inline SettingsStore() noexcept = default;

  void reloadUserSettings();

  [[nodiscard]] constexpr settings::UserSettingsDocument& user() noexcept {
    return user_settings_;
  }
  [[nodiscard]] constexpr settings::UserSettingsDocument const& user() const noexcept {
    return user_settings_;
  }

private:
  settings::UserSettingsDocument user_settings_;
};

}
