#include "pch.h"
#include "MoveAndResizeOperation.h"

#include "../context/OperationContext.h"
#include "../monitor/MonitorStateCache.h"
#include "../win32/monitor.h"
#include "../window/WindowStateCache.h"

using namespace roxyg::engine;
using namespace roxyg::numeric;

static winrt::hresult applyWindowRect(HWND hwnd, long4 bounds) noexcept {
  BOOL rc = SetWindowPos(
    hwnd,
    nullptr,
    bounds.x(), bounds.y(), bounds.z(), bounds.w(),
    SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_ASYNCWINDOWPOS);
  if (!rc) [[unlikely]] {
    return winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
  }

  return S_OK;
}

winrt::hresult AbsoluteMoveAndResizeOperation::execute([[maybe_unused]] OperationContext& _, window::State& windowState) noexcept {
  return applyWindowRect(windowState.hWnd(), bounds_);
}

winrt::hresult RelativeMoveAndResizeOperation::execute(OperationContext& ctx, window::State& windowState) noexcept {
  if (dirty_) [[unlikely]] {
    HMONITOR hMonitor = win32::GetPrimaryHMonitor();

    // Calc the new window bounds from relative bounds.
    float4 const workArea{ctx.monitor().getOrCreate(hMonitor)->workArea()};
    float2_fast const workSize = workArea.zw();
    float2_fast const expectedSize = (relative_bounds_.zw() * workSize).ceil();
    float2_fast const expectedPos = workArea.xy() + (relative_bounds_.xy() * (workSize - expectedSize)).floor();
    float4 const expectedBounds = float4::concat(expectedPos, expectedSize) + windowState.shadowMargin();

    bounds_ = ltrunc_cast(expectedBounds);
    dirty_ = false;
  }

  return applyWindowRect(windowState.hWnd(), bounds_);
}
