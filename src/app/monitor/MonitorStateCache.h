#pragma once
#include "MonitorState.h"

#include "../message/IMessageListener.h"
#include "../logging/LogHelper.h"
#include "../utility/CallbackHost.h"
#include "../utility/ValueCache.h"

namespace roxyg::monitor {

/// <summary>
/// Caches monitor states by monitor handle.
/// </summary>
/// <remarks>
/// Before invalidating or rebuilding the cache, ensure that all workers
/// accessing monitor states have completed.
/// </remarks>
class StateCache final
  : public utility::ValueCache<HMONITOR, State>
  , public utility::CallbackHost<StateCache>
  , public roxyg::message::IMessageListener {
public:
  [[nodiscard]] winrt::hresult initialize() noexcept;

  [[nodiscard]] State const* findById(int id) const noexcept;

  constexpr void setLogger(logging::Logger* logger) noexcept {
    log_.setLogger(logger);
  }

private:
  void onDisplayChanged() noexcept override;

  static BOOL CALLBACK EnumDisplayMonitorsCallback(HMONITOR hmonitor, HDC, LPRECT lprcMonitor, LPARAM data) noexcept;

private:
  logging::LogHelper<logging::LogGroup::kMonitor> log_;
};

}
