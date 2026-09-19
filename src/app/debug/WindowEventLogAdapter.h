#pragma once

#include "../logging/LogHelper.h"
#include "../window/IWindowStateListener.h"

namespace roxyg::debug {

class WindowEventLogAdapter final
  : public window::IWindowStateListener {
  WindowEventLogAdapter(WindowEventLogAdapter const&) = delete;
  WindowEventLogAdapter& operator=(WindowEventLogAdapter const&) = delete;

public:
  constexpr WindowEventLogAdapter() noexcept = default;

  void onForegroundEvent(window::State& state) noexcept override {
    handleEvent(state, L"EVENT_SYSTEM_FOREGROUND");
  }

  void onShowEvent(window::State& state) noexcept override {
    handleEvent(state, L"EVENT_OBJECT_SHOW");
  }

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

private:
  void handleEvent(window::State& state, std::wstring_view eventName) noexcept;

private:
  logging::LogHelper<logging::LogGroup::kDebug> logger_;
};

}
