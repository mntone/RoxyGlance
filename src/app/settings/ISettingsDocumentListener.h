#pragma once

namespace roxyg::settings {

template<typename TDocument>
struct ISettingsDocumentListener {
  virtual void onSettingsChanged(TDocument const& settings) noexcept = 0;

protected:
  ~ISettingsDocumentListener() = default;
};

class UserSettingsDocument;

using IUserSettingsDocumentListener = ISettingsDocumentListener<UserSettingsDocument>;

}
