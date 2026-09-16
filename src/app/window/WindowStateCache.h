#pragma once
#include "WindowState.h"

#include "../utility/ValueCache.h"

namespace roxyg::window {

/// <summary>
/// Caches window states by window handle.
/// </summary>
/// <remarks>
/// Before invalidating or rebuilding the cache, ensure that all workers
/// accessing window states have completed.
/// </remarks>
class StateCache final
  : public utility::ValueCache<HWND, State> {
public:
  constexpr StateCache() noexcept {
    storage_.reserve(16);
  }

  void processWindowEvent(DWORD event, HWND hwnd) noexcept;
};

}
