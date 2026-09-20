#pragma once

namespace roxyg::win32::hresult {

__forceinline ::winrt::hresult LastErrorAsHResult() noexcept {
  return ::winrt::impl::hresult_from_win32(WINRT_IMPL_GetLastError());
}

}
