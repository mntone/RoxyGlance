#pragma once
#include "logging/Logger.h"
#include "message/IMessageListener.h"

namespace roxyg {

class AppContext;
class AppDelegate final {
  AppDelegate(AppDelegate const&) = delete;
  AppDelegate& operator=(AppDelegate const&) = delete;

public:
  AppDelegate() noexcept;
  ~AppDelegate() noexcept;

  [[nodiscard]] winrt::hresult initialize();
  [[nodiscard]] winrt::hresult exit() noexcept;

  [[nodiscard]] winrt::hresult addMessageListener(message::IMessageListener* listener) noexcept;
  winrt::hresult removeMessageListener(message::IMessageListener* listener) noexcept;

  [[nodiscard]] logging::Logger& logger() noexcept;
  [[nodiscard]] logging::Logger const& logger() const noexcept;

  constexpr AppContext& context() noexcept { return *context_; }
  constexpr AppContext const& context() const noexcept { return *context_; }

private:
  std::unique_ptr<AppContext> context_;
};

}
