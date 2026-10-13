#pragma once

#ifdef __INTELLISENSE__
#include <winrt/Windows.Foundation.h>
#endif

namespace roxyg::winui3::viewmodel {

namespace detail {
struct StaticCommandHolderBase {
  bool CanExecute(::winrt::Windows::Foundation::IInspectable const&) const noexcept;

  ::winrt::event_token CanExecuteChanged(
    ::winrt::Windows::Foundation::EventHandler<::winrt::Windows::Foundation::IInspectable> const&
  ) const noexcept;
  void CanExecuteChanged(::winrt::event_token) const noexcept;
};
}

template<typename Derived>
struct StaticCommandHolder: public detail::StaticCommandHolderBase {
  using ActionFunction = void(*)(Derived& self, ::winrt::Windows::Foundation::IInspectable const& param);

  StaticCommandHolder(std::nullptr_t) = delete;
  explicit inline constexpr StaticCommandHolder(ActionFunction action) noexcept
    : action_(action) {
    ROXYG_UNCHECKED_ASSERT(action_);
  }

  inline void Execute(::winrt::Windows::Foundation::IInspectable const& parameter) {
    Derived* const self{static_cast<Derived*>(this)};
    action_(*self->get_strong(), parameter);
  }

private:
  ActionFunction const action_;
};

}
