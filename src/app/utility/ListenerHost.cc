#include "pch.h"
#include "ListenerHost.h"

using namespace roxyg::utility;

bool _ListenerHostBase::addListener(void* listener) {
#if _DEBUG
  if (!listener) [[unlikely]] {
    throw winrt::hresult_invalid_argument(L"nullptr");
  }
#else
  [[assume(listener != nullptr)]];
#endif


  return listeners_.stable_emplace_back(listener);
}

bool _ListenerHostBase::removeListener(void* listener) {
#if _DEBUG
  if (!listener) [[unlikely]] {
    throw winrt::hresult_invalid_argument(L"nullptr");
  }
#else
  [[assume(listener != nullptr)]];
#endif

  ContainerType::iterator it = std::remove(listeners_.begin(), listeners_.end(), listener);
  if (it == listeners_.end()) [[unlikely]] {
    return false;
  }

  listeners_.erase(it, listeners_.end());
  return true;
}
