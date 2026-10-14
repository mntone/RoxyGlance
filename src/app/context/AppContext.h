#pragma once
#include "OperationContext.h"

#ifdef _DEBUG
#include "../debug/WindowEventLogAdapter.h"
#endif
#include "../engine/RuleEngine.h"
#include "../logging/Logger.h"
#include "../message/MessageWindow.h"
#include "../settings/SettingsStore.h"

namespace roxyg {

class AppContext final {
  friend class AppDelegate;

  AppContext(AppContext const&) = delete;
  AppContext& operator=(AppContext const&) = delete;

public:
  AppContext() noexcept;
  ~AppContext() noexcept;

  [[nodiscard]] winrt::hresult initialize();
  [[nodiscard]] winrt::hresult exit() noexcept;

private:
  OperationContext operation_context_;
  logging::Logger logger_;
  engine::RuleEngine engine_;
  settings::SettingsStore settings_store_;  // <-- require this after engine!
  message::Window message_window_;
  logging::LogHelper<logging::LogGroup::kContext> context_logger_;

#ifdef _DEBUG
  debug::WindowEventLogAdapter log_adapter_;
#endif
};

}
