#pragma once
#include "ViewModels/Settings/StringFilterViewModel.g.h"

#include "../Holders/CallbackHolder.h"
#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../Holders/StaticCommandHolder.h"
#include "../../../app/settings/Filter.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct StringFilterViewModel
  : StringFilterViewModelT<StringFilterViewModel, Microsoft::UI::Xaml::Input::ICommand, non_agile>
  , roxyg::winui3::viewmodel::CallbackHolder<FilterKey>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<StringFilterViewModel>
  , roxyg::winui3::viewmodel::StaticCommandHolder<StringFilterViewModel> {
  explicit StringFilterViewModel(
    roxyg::settings::Filter& filter,
    FilterKey key,
    CallbackFunction callback,
    IInspectable const& context,
    bool new_elem = false
  );

  inline Microsoft::UI::Xaml::Input::ICommand DeleteCommand() const noexcept {
    return *this;
  }

  inline FilterKey Key() const noexcept {
    return Key_;
  }

  DEFINE_PROP(StringMatchType, ValueMatchType);
  DEFINE_VALPROP(hstring, Value);

  void save();

  static void initialize();

private:
  static void onDelete(StringFilterViewModel& self, Windows::Foundation::IInspectable const& param);

private:
  roxyg::settings::Filter& filter_;

  FilterKey const Key_;
  StringMatchType ValueMatchType_;
  hstring Value_;
  bool dirty_;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ValueMatchPropertyChangedEventArgs_;
  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs ValuePropertyChangedEventArgs_;
};

}
