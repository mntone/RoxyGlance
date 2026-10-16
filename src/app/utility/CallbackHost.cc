#include "pch.h"
#include "CallbackHost.h"

using namespace roxyg::utility;

winrt::hresult detail::CallbackHostBase::addCallback(RawCallbackFunction callback, Context context) noexcept {
  ROXYG_UNCHECKED_ASSERT(callback);
  ROXYG_UNCHECKED_ASSERT(context);

  std::lock_guard<win32::SRWLock> guard(lock_);
  return callbacks_.stable_emplace_back(callback, context) ? S_OK : E_UNEXPECTED;
}

winrt::hresult detail::CallbackHostBase::removeCallback(RawCallbackFunction callback, Context context) noexcept {
  ROXYG_UNCHECKED_ASSERT(context);

  std::lock_guard<win32::SRWLock> guard(lock_);
  RawContainerType::const_iterator it = std::remove(
    callbacks_.begin(),
    callbacks_.end(),
    std::make_pair(callback, context)
  );
  if (it == callbacks_.end()) [[unlikely]] {
    return S_FALSE;
  }

  callbacks_.erase(it, callbacks_.end());
  return S_OK;
}
