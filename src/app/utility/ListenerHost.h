#pragma once

namespace roxyg::utility {

struct _ListenerHostBase {
protected:
  using ContainerType = boost::container::static_vector<void*, 3>;
  ContainerType listeners_;

  bool addListener(void* listener);
  bool removeListener(void* listener);
};

template<typename Listener>
struct ListenerHost: public _ListenerHostBase {
public:
  __forceinline bool addListener(Listener* listener) {
    return _ListenerHostBase::addListener(listener);
  }

  __forceinline bool removeListener(Listener* listener) {
    return _ListenerHostBase::removeListener(listener);
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
