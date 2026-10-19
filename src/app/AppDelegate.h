#pragma once

namespace roxyg {

namespace logging { class Logger; }
namespace message { class Window; }
namespace monitor { class StateCache; }
namespace settings { class UserSettingsStore; }

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

  [[nodiscard]] monitor::StateCache& monitorStateCache() noexcept;
  [[nodiscard]] monitor::StateCache const& monitorStateCache() const noexcept;

  [[nodiscard]] settings::UserSettingsStore& settingsStore() noexcept;
  [[nodiscard]] settings::UserSettingsStore const& settingsStore() const noexcept;

  constexpr AppContext& context() noexcept { return *context_; }
  constexpr AppContext const& context() const noexcept { return *context_; }

private:
  std::unique_ptr<AppContext> context_;
};

}
