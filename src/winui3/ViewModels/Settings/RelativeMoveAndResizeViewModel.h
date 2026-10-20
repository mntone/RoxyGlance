#pragma once
#include "ActionViewModel.h"
#include "ViewModels/Settings/RelativeMoveAndResizeViewModel.g.h"
#include "MoveAndResizeViewModelProperties.h"
#include "SettingsViewModelProperties.h"

#include "../Holders/CallbackHolder.h"
#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/action/MoveAndResize.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct RelativeMoveAndResizeViewModel
  : RelativeMoveAndResizeViewModelT<RelativeMoveAndResizeViewModel, ActionViewModel>
  , roxyg::winui3::viewmodel::CallbackHolder<>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<RelativeMoveAndResizeViewModel>
  , MoveAndResizeViewModelProperties
  , SettingsViewModelProperties {
  RelativeMoveAndResizeViewModel(
    roxyg::settings::action::RelativeMoveAndResize action,
    CallbackFunction callback,
    IInspectable const& context,
    bool change_type
  );

  DEFINE_PROP(int, Id);
  //DEFINE_PROP(hstring, Name);
  DEFINE_PROP(float, X);
  DEFINE_PROP(float, Y);
  DEFINE_PROP(float, Width);
  DEFINE_PROP(float, Height);

  bool HasPendingChanges() const noexcept override final;
  bool applyChanges() override final;

  static void initialize();

protected:
  void HasPendingChanges(bool value) override final;

private:
  roxyg::settings::action::RelativeMoveAndResize action_;

  //hstring Name_;
  int Id_;
  float X_, Y_, Width_, Height_;

  bool HasPendingChanges_;
  bool Id_dirty_ : 1;
  //bool Name_dirty_ : 1;
  bool X_dirty_ : 1;
  bool Y_dirty_ : 1;
  bool Width_dirty_ : 1;
  bool Height_dirty_ : 1;

  static Microsoft::UI::Xaml::Data::PropertyChangedEventArgs IdPropertyChangedEventArgs_;
};

}
