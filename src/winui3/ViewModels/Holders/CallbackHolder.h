#pragma once

namespace roxyg::winui3::viewmodel {

template<typename Derived>
struct CallbackHolder {
  using Context = ::winrt::Windows::Foundation::IInspectable;
  using CallbackFunction = void(*)(Derived const& sender, Context const& context);

  CallbackHolder(::std::nullptr_t, Context const&) = delete;
  explicit inline constexpr CallbackHolder(CallbackFunction callback, Context const& context) noexcept
    : callback_(callback)
    , context_(context) {
    ROXYG_UNCHECKED_ASSERT(callback_);
  }

protected:
  void invokeCallback() {
    Context context{context_.get()};
    if (context) {
      Derived* const self{static_cast<Derived*>(this)};
      callback_(*self->get_strong(), context);
    }
  }

protected:
  CallbackFunction const callback_;
  ::winrt::weak_ref<Context> const context_;
};

}
