#pragma once

#ifdef __INTELLISENSE__
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#endif

namespace roxyg::winui3::viewmodel {

namespace detail {
struct NotifyPropertyChangedHolderBase {
  ::winrt::event_token PropertyChanged(::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
  void PropertyChanged(::winrt::event_token const token) noexcept;

protected:
  ::winrt::event<::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> property_changed_;
};
}

template<typename Derived>
struct NotifyPropertyChangedHolder: public detail::NotifyPropertyChangedHolderBase {
protected:
  inline void notifyPropertyChanged(::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
    Derived* const self{static_cast<Derived*>(this)};
    property_changed_(*self->get_strong(), args);
  }
  inline void notifyPropertyChanged(::winrt::param::hstring property_name) {
    notifyPropertyChanged(::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{property_name});
  }

  template<typename T>
  inline bool setProperty(T& storage, T const& value, ::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& args) {
    if (storage == value) {
      return false;
    }

    storage = value;
    notifyPropertyChanged(args);
    return true;
  }
  template<typename T>
  inline bool setProperty(T& storage, T const& value, ::winrt::param::hstring property_name) {
    return setProperty(storage, value, ::winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs{property_name});
  }
};

}

#define DEFINE_PROP(Type, name) \
  inline constexpr Type name() const noexcept { \
    return name##_; \
  } \
  void name(Type value);
#define DEFINE_IINSPECTABLE_PROP(Type, name) \
  inline Type name() const noexcept { \
    return name##_; \
  } \
  void name(Type const& value);
