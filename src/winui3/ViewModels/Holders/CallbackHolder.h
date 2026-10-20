#pragma once

namespace roxyg::winui3::viewmodel {

template<typename... Args>
struct CallbackHolder {
  using Context = ::winrt::Windows::Foundation::IInspectable;
  using CallbackFunction = void(*)(Context const& context, Args... args);

  CallbackHolder(::std::nullptr_t, Context const&) = delete;
  explicit inline constexpr CallbackHolder(CallbackFunction callback, Context const& context) noexcept
    : callback_(callback)
    , context_(context) {
    ROXYG_UNCHECKED_ASSERT(callback_);
  }

protected:
  void invokeCallback(Args... args) {
    Context context{context_.get()};
    if (context) {
      callback_(context, std::forward<Args>(args)...);
    }
  }

protected:
  CallbackFunction const callback_;
  ::winrt::weak_ref<Context> const context_;
};

}
