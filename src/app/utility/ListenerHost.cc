#include "pch.h"
#include "ListenerHost.h"

using namespace roxyg::utility;

winrt::hresult detail::ListenerHostBase::addListener(void* listener) noexcept {
  ROXYG_UNCHECKED_ASSERT(listener);

  return listeners_.stable_emplace_back(listener) ? S_OK : E_UNEXPECTED;
}

winrt::hresult detail::ListenerHostBase::removeListener(void* listener) noexcept {
  ROXYG_UNCHECKED_ASSERT(listener);

  ContainerType::const_iterator it = std::remove(listeners_.begin(), listeners_.end(), listener);
  if (it == listeners_.end()) [[unlikely]] {
    return S_FALSE;
  }

  listeners_.erase(it, listeners_.end());
  return S_OK;
}
