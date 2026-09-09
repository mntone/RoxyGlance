#pragma once

namespace roxyg::settings {

class UserSettingsDocument;
struct ISettingsListener {
  virtual void onSettingsChanged(UserSettingsDocument const& settings) noexcept = 0;
};

}
