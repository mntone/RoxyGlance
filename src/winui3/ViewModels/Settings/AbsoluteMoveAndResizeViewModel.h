#pragma once
#include "ActionViewModel.h"
#include "ViewModels/Settings/AbsoluteMoveAndResizeViewModel.g.h"
#include "MoveAndResizeViewModelProperties.h"
#include "SettingsViewModelProperties.h"

#include "../Holders/CallbackHolder.h"
#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/action/MoveAndResize.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct AbsoluteMoveAndResizeViewModel
  : AbsoluteMoveAndResizeViewModelT<AbsoluteMoveAndResizeViewModel, ActionViewModel>
  , roxyg::winui3::viewmodel::CallbackHolder<>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<AbsoluteMoveAndResizeViewModel>
  , MoveAndResizeViewModelProperties
  , SettingsViewModelProperties {
  AbsoluteMoveAndResizeViewModel(
    roxyg::settings::action::AbsoluteMoveAndResize action,
    CallbackFunction callback,
    IInspectable const& context,
    bool change_type
  );

  DEFINE_PROP(int32_t, X);
  DEFINE_PROP(int32_t, Y);
  DEFINE_PROP(int32_t, Width);
  DEFINE_PROP(int32_t, Height);

  bool HasPendingChanges() const noexcept override final;
  bool applyChanges() override final;

protected:
  void HasPendingChanges(bool value) override final;

private:
  roxyg::settings::action::AbsoluteMoveAndResize action_;

  int32_t X_, Y_, Width_, Height_;
  bool X_dirty_;
  bool Y_dirty_;
  bool Width_dirty_;
  bool Height_dirty_;
  bool HasPendingChanges_;
};

}
