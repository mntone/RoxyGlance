#pragma once

namespace roxyg::utility {

namespace detail {
struct ListenerHostBase {
protected:
  using ContainerType = boost::container::static_vector<void*, 3>;
  ContainerType listeners_;

  winrt::hresult addListener(void* listener) noexcept;
  winrt::hresult removeListener(void* listener) noexcept;
};
}

template<typename Listener>
struct ListenerHost: public detail::ListenerHostBase {
public:
  [[nodiscard]] ROXYG_ALWAYS_INLINE winrt::hresult addListener(Listener* listener) noexcept {
    return detail::ListenerHostBase::addListener(listener);
  }

  ROXYG_ALWAYS_INLINE winrt::hresult removeListener(Listener* listener) noexcept {
    return detail::ListenerHostBase::removeListener(listener);
  }

  template<typename Func, typename... Args>
  void notify(Func func, Args&&... args)
    noexcept(std::is_nothrow_invocable_v<Func, Listener&, Args&&...>) {
    for (void* ptr : listeners_) {
      Listener* listener = static_cast<Listener*>(ptr);
      (listener->*func)(std::forward<Args>(args)...);
    }
  }
  template<typename Func, typename... Args>
  void notify(Func func, Args&&... args) const
    noexcept(std::is_nothrow_invocable_v<Func, Listener const&, Args&&...>) {
    for (void* ptr : listeners_) {
      Listener const* listener = static_cast<Listener const*>(ptr);
      (listener->*func)(std::forward<Args>(args)...);
    }
  }
};

}
