#pragma once
#include "OperationContext.h"

#if _DEBUG
#include "../debug/WindowEventLogAdapter.h"
#endif
#include "../engine/RuleEngine.h"
#include "../logging/Logger.h"
#include "../message/MessageWindow.h"
#include "../settings/SettingsStore.h"

namespace roxyg {

class AppContext final {
  AppContext(AppContext const&) = delete;
  AppContext& operator=(AppContext const&) = delete;

public:
  AppContext() noexcept;
  ~AppContext() noexcept;

  [[nodiscard]] winrt::hresult initialize();
  [[nodiscard]] winrt::hresult exit() noexcept;

  [[nodiscard]] constexpr logging::Logger& logger() noexcept {
    return logger_;
  }
  [[nodiscard]] constexpr logging::Logger const& logger() const noexcept {
    return logger_;
  }

  [[nodiscard]] constexpr message::Window& messageWindow() noexcept {
    return message_window_;
  }
  [[nodiscard]] constexpr message::Window const& messageWindow() const noexcept {
    return message_window_;
  }

  [[nodiscard]] constexpr settings::SettingsStore const& settingsStore() const noexcept {
    return settings_store_;
  }

private:
  OperationContext operation_context_;
  logging::Logger logger_;
  engine::RuleEngine engine_;
  settings::SettingsStore settings_store_;  // <-- require this after engine!
  message::Window message_window_;
  logging::LogHelper<logging::LogGroup::kContext> context_logger_;

#if _DEBUG
  debug::WindowEventLogAdapter log_adapter_;
#endif
};

}
