#pragma once
#include "ActionViewModel.h"
#include "ViewModels/Settings/AbsoluteMoveAndResizeViewModel.g.h"
#include "MoveAndResizeViewModelProperties.h"

#include "../Holders/NotifyPropertyChangedHolder.h"
#include "../../../app/settings/action/MoveAndResize.h"

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

struct AbsoluteMoveAndResizeViewModel
  : AbsoluteMoveAndResizeViewModelT<AbsoluteMoveAndResizeViewModel, ActionViewModel>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<AbsoluteMoveAndResizeViewModel>
  , MoveAndResizeViewModelProperties {
  AbsoluteMoveAndResizeViewModel(roxyg::settings::action::AbsoluteMoveAndResize action);

  DEFINE_PROP(int32_t, X);
  DEFINE_PROP(int32_t, Y);
  DEFINE_PROP(int32_t, Width);
  DEFINE_PROP(int32_t, Height);

  void save() override final;

private:
  roxyg::settings::action::AbsoluteMoveAndResize action_;

  int32_t X_, Y_, Width_, Height_;
  bool X_dirty_;
  bool Y_dirty_;
  bool Width_dirty_;
  bool Height_dirty_;
};

}
