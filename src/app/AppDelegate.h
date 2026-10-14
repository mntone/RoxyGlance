#pragma once
#include "message/IMessageListener.h"

namespace roxyg {

namespace logging { class Logger; }
namespace message { class Window; }

class AppContext;
class AppDelegate final {
  AppDelegate(AppDelegate const&) = delete;
  AppDelegate& operator=(AppDelegate const&) = delete;

public:
  AppDelegate() noexcept;
  ~AppDelegate() noexcept;

  [[nodiscard]] winrt::hresult initialize();
  [[nodiscard]] winrt::hresult exit() noexcept;

  [[nodiscard]] logging::Logger& logger() noexcept;
  [[nodiscard]] logging::Logger const& logger() const noexcept;

  [[nodiscard]] message::Window& messageWindow() noexcept;
  [[nodiscard]] message::Window const& messageWindow() const noexcept;

  constexpr AppContext& context() noexcept { return *context_; }
  constexpr AppContext const& context() const noexcept { return *context_; }

private:
  std::unique_ptr<AppContext> context_;
};

}
