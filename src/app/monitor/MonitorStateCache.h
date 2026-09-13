#pragma once
#include "MonitorState.h"

namespace roxyg::monitor {

inline constexpr std::size_t kMonitorStateCacheDefaultSize = 4;

/// <summary>
/// Caches monitor states by monitor handle.
/// </summary>
/// <remarks>
/// State objects are stored by value. Pointers and references obtained from
/// this cache are valid only until the next modification of the cache. They
/// must not be retained or used concurrently with cache modification.
///
/// Adding or removing a state may reallocate the underlying flat map and
/// invalidate pointers and references to cached states. Before invalidating or
/// rebuilding the cache, ensure that all workers accessing cached states have
/// completed.
/// </remarks>
class StateCache final {
  StateCache(StateCache const&) = delete;
  StateCache& operator=(StateCache const&) = delete;

public:
  using StatesType = boost::unordered_flat_map<HMONITOR, State>;

  /// <summary>
  /// Initializes an empty monitor state cache.
  /// </summary>
  constexpr StateCache() noexcept {
    states_.reserve(kMonitorStateCacheDefaultSize);
  }

  [[nodiscard]] winrt::hresult initialize() noexcept;

  /// <summary>
  /// Gets the cached state for the specified monitor.
  /// </summary>
  /// <param name="hmonitor">The handle of the monitor.</param>
  /// <returns>
  /// A pointer to the cached state, or <c>nullptr</c> if no state exists.
  /// The returned pointer must not be retained.
  /// </returns>
  [[nodiscard]] inline State* get(HMONITOR hmonitor) noexcept {
    StatesType::iterator it = states_.find(hmonitor);
    return it != states_.end() ? &it->second : nullptr;
  }

  /// <summary>
  /// Gets the cached state for the specified monitor, creating it if necessary.
  /// </summary>
  /// <param name="hmonitor">The handle of the monitor.</param>
  /// <param name="state">
  /// Receives a pointer to the cached state. Set to <c>nullptr</c> if memory
  /// allocation fails.
  /// </param>
  /// <returns>
  /// <c>true</c> if a new state was created; <c>false</c> if the state already
  /// existed or memory allocation failed.
  /// </returns>
  /// <remarks>
  /// The pointer returned through <paramref name="state"/> must not be retained
  /// or used concurrently with any modification of this cache.
  ///
  /// If a state is created, insertion may reallocate the underlying flat map and
  /// invalidate pointers and references to cached states.
  ///
  /// When the return value is <c>false</c>, a non-null pointer indicates that the
  /// state already existed; <c>nullptr</c> indicates that allocation failed.
  /// </remarks>
  inline bool getOrCreate(HMONITOR hmonitor, State*& state) noexcept {
    try {
      auto [it, emplaced] = states_.try_emplace(hmonitor, hmonitor);
      state = &it->second;
      return emplaced;
    } catch (std::bad_alloc const&) {
      state = nullptr;
      return false;
    }
  }

  /// <summary>
  /// Removes the cached state for the specified monitor.
  /// </summary>
  /// <param name="hmonitor">The handle of the monitor.</param>
  /// <remarks>
  /// This operation may invalidate pointers and references to cached states.
  /// </remarks>
  inline void remove(HMONITOR hmonitor) noexcept {
    states_.erase(hmonitor);
  }

private:
  static BOOL CALLBACK EnumDisplayMonitorsCallback(HMONITOR hmonitor, HDC, LPRECT lprcMonitor, LPARAM data) noexcept;

private:
  StatesType states_;
};

}
