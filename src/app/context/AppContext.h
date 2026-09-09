#pragma once
#include "OperationContext.h"

#if _DEBUG
#include "../debug/WindowEventLogAdapter.h"
#endif
#include "../engine/RuleEngine.h"
#include "../logging/ILogger.h"
#include "../logging/ILogSource.h"
#include "../settings/SettingsStore.h"

namespace roxyg {

class AppContext final {
  AppContext(AppContext const&) = delete;
  AppContext& operator=(AppContext const&) = delete;

public:
  AppContext() noexcept;

  winrt::hresult initialize(
    std::shared_ptr<logging::ILogger> logger,
    std::shared_ptr<logging::ILogSource> logSource
  ) noexcept;

  [[nodiscard]] inline std::shared_ptr<logging::ILogSource> logSource() const noexcept {
    return log_source_;
  }

  [[nodiscard]] constexpr settings::SettingsStore const& settingsStore() const noexcept {
    return settings_store_;
  }

private:
  OperationContext operation_context_;
  engine::RuleEngine engine_;
  settings::SettingsStore settings_store_;  // <-- require this after engine!
  std::shared_ptr<logging::ILogSource> log_source_;

#if _DEBUG
  debug::WindowEventLogAdapter log_adapter_;
#endif
};

}
