#pragma once

#ifdef __INTELLISENSE__
#include <type_traits>
#include <utility>
#include <winrt/base.h>
#endif

namespace roxyg::winrt {

namespace detail {
template<typename Handler, typename T, typename... Args>
constexpr bool is_weak_delegate_nothrow_v = [] {
  if constexpr (std::is_invocable_v<Handler&, T const&, Args...>) {
    return std::is_nothrow_invocable_v<Handler&, T const&, Args...>;
  } else if constexpr (std::is_invocable_v<Handler&, T&, Args...>) {
    return std::is_nothrow_invocable_v<Handler&, T&, Args...>;
  } else {
    return std::is_nothrow_invocable_v<Handler&, Args...>;
  }
}();
}

/// <summary>
/// Creates a delegate/callback that holds a weak reference to the target object.
/// If the target is still alive, invokes the handler; otherwise, does nothing.
/// </summary>
/// <typeparam name="T">The type of the target object.</typeparam>
/// <typeparam name="Handler">The callable type.</typeparam>
/// <param name="weak">Weak reference to the target.</param>
/// <param name="handler">Handler to execute.</param>
template<typename T, typename Handler>
auto weak_delegate(::winrt::weak_ref<T> weak, Handler handler) noexcept(
  std::is_nothrow_move_constructible_v<::winrt::weak_ref<T>> &&
  std::is_nothrow_move_constructible_v<Handler>
) {
  return [weak = std::move(weak), handler = std::move(handler)](auto&&... args) mutable
    noexcept(detail::is_weak_delegate_nothrow_v<Handler, T, decltype(args)...>) -> void {
    if (T strong = weak.get()) {
      if constexpr (std::is_invocable_v<Handler&, T const&, decltype(args)...>) {
        handler(strong, std::forward<decltype(args)>(args)...);
      } else if constexpr (std::is_invocable_v<Handler&, T&, decltype(args)...>) {
        handler(strong, std::forward<decltype(args)>(args)...);
      } else {
        handler(std::forward<decltype(args)>(args)...);
      }
    }
  };
}

/// <summary>
/// Creates a weak delegate directly from a strong reference or WinRT projected type.
/// </summary>
/// <param name="strong">Strong reference to the target.</param>
/// <param name="handler">Handler to execute.</param>
template<typename T, typename Handler>
auto weak_delegate(T const& strong, Handler handler) noexcept(
  noexcept(weak_delegate(::winrt::make_weak(strong), std::move(handler)))
) {
  return weak_delegate(::winrt::make_weak(strong), std::move(handler));
}

/// <summary>
/// Creates a weak delegate targeting a member function pointer.
/// e.g. weak_delegate(this, &MyClass::OnClicked)
/// </summary>
template<typename T, typename Class, typename Ret, typename... Args>
auto weak_delegate(T const& target, Ret (Class::*method)(Args...) noexcept) noexcept {
  return weak_delegate(target, [method](T& strong, Args... args) noexcept {
    (strong.*method)(std::forward<Args>(args)...);
  });
}

/// <summary>
/// Creates a weak delegate targeting a member function pointer.
/// e.g. weak_delegate(this, &MyClass::OnClicked)
/// </summary>
template<typename T, typename Class, typename Ret, typename... Args>
auto weak_delegate(T const& target, Ret (Class::*method)(Args...)) {
  return weak_delegate(target, [method](T& strong, Args... args) {
    (strong.*method)(std::forward<Args>(args)...);
  });
}

}  // namespace roxyg::winrt
