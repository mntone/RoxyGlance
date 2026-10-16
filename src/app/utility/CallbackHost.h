#pragma once
#include "../win32/SRWLock.h"

#ifdef __INTELLISENSE__
#include <mutex>
#include <boost/container/static_vector.hpp>
#endif

namespace roxyg::utility {

namespace detail {
struct CallbackHostBase {
  using Context = void*;

protected:
  using RawCallbackFunction = void(*)() noexcept;
  using RawCallbackData = std::pair<RawCallbackFunction, Context>;
  using RawContainerType = boost::container::static_vector<RawCallbackData, 3>;
  RawContainerType callbacks_;
  win32::SRWLock lock_;

  winrt::hresult addCallback(RawCallbackFunction callback, Context context) noexcept;
  winrt::hresult removeCallback(RawCallbackFunction callback, Context context) noexcept;
};
}

template<typename Derived, typename... Args>
struct CallbackHost: public detail::CallbackHostBase {
  using CallbackFunction = void(*)(Derived const& sender, Context context, Args&&... args) noexcept;

  /// <summary>
  /// Registers a callback to be invoked by this host.
  /// </summary>
  /// <param name="callback">The callback function to register.</param>
  /// <param name="context">The context passed to the callback and used to identify the registration.</param>
  /// <returns>S_OK if the callback was registered; E_UNEXPECTED if the callback capacity is exhausted.</returns>
  /// <remarks>
  /// This method waits for any in-progress callback invocation to complete. Do not call it from a callback invoked by this host,
  /// because the invocation holds the same non-recursive lock.
  /// </remarks>
  [[nodiscard]] ROXYG_ALWAYS_INLINE winrt::hresult addCallback(CallbackFunction callback, Context context) noexcept {
    return detail::CallbackHostBase::addCallback(reinterpret_cast<RawCallbackFunction>(callback), context);
  }
  winrt::hresult addCallback(std::nullptr_t, Context) = delete;
  winrt::hresult addCallback(CallbackFunction, std::nullptr_t) = delete;

  /// <summary>
  /// Removes registrations matching the callback and context.
  /// </summary>
  /// <param name="callback">The callback function to match.</param>
  /// <param name="context">The context to match.</param>
  /// <returns>S_OK if one or more matching registrations were removed; S_FALSE if no matching registration was found.</returns>
  /// <remarks>
  /// This method waits for any in-progress callback invocation to complete. Do not call it from a callback invoked by this host,
  /// because the invocation holds the same non-recursive lock.
  /// </remarks>
  ROXYG_ALWAYS_INLINE winrt::hresult removeCallback(CallbackFunction callback, Context context) noexcept {
    return detail::CallbackHostBase::removeCallback(reinterpret_cast<RawCallbackFunction>(callback), context);
  }
  winrt::hresult removeCallback(std::nullptr_t, Context) = delete;
  winrt::hresult removeCallback(CallbackFunction, std::nullptr_t) = delete;

protected:
  inline void invoke(Args&&... args) noexcept {
    std::lock_guard<win32::SRWLock> guard(lock_);
    Derived const& sender{*static_cast<Derived*>(this)};
    for (RawCallbackData const& data : callbacks_) {
      CallbackFunction const callback{reinterpret_cast<CallbackFunction>(data.first)};
      callback(sender, data.second, std::forward<Args>(args)...);
    }
  }
};

}
