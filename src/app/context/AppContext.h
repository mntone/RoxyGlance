#pragma once
#include "OperationContext.h"

#if _DEBUG
#include "../debug/WindowEventLogAdapter.h"
#endif
#include "../engine/RuleEngine.h"
#include "../logging/Logger.h"
#include "../settings/SettingsStore.h"

namespace roxyg {

class AppContext final {
  AppContext(AppContext const&) = delete;
  AppContext& operator=(AppContext const&) = delete;

public:
  AppContext() noexcept;
  ~AppContext() noexcept;

  winrt::hresult initialize();

  [[nodiscard]] constexpr logging::Logger& logger() noexcept {
    return logger_;
  }
  [[nodiscard]] constexpr logging::Logger const& logger() const noexcept {
    return logger_;
  }

  [[nodiscard]] constexpr settings::SettingsStore const& settingsStore() const noexcept {
    return settings_store_;
  }

private:
  OperationContext operation_context_;
  logging::Logger logger_;
  engine::RuleEngine engine_;
  settings::SettingsStore settings_store_;  // <-- require this after engine!

#if _DEBUG
  debug::WindowEventLogAdapter log_adapter_;
#endif
};

}
