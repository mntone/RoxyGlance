#include "pch.h"
#include "AbsoluteMoveAndResizeViewModel.h"
#if __has_include("ViewModels/Settings/AbsoluteMoveAndResizeViewModel.g.cpp")
#include "ViewModels/Settings/AbsoluteMoveAndResizeViewModel.g.cpp"
#endif

#include "../../../app/settings/constants.h"

using namespace roxyg::settings::action;
using namespace roxyg::settings::numeric_limit;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

AbsoluteMoveAndResizeViewModel::AbsoluteMoveAndResizeViewModel(
  AbsoluteMoveAndResize action,
  CallbackFunction callback,
  IInspectable const& context,
  bool change_type
)
  : base_type(ActionType::AbsoluteMoveAndResize)
  , CallbackHolder(callback, context)
  , action_(std::move(action))
  , X_(action.windowBounds().x())
  , Y_(action.windowBounds().y())
  , Width_(action.windowBounds().z())
  , Height_(action.windowBounds().w())
  , HasPendingChanges_(change_type)
  , X_dirty_(false)
  , Y_dirty_(false)
  , Width_dirty_(false)
  , Height_dirty_(false) {
}

bool AbsoluteMoveAndResizeViewModel::HasPendingChanges() const noexcept {
  return HasPendingChanges_;
}

void AbsoluteMoveAndResizeViewModel::HasPendingChanges(bool value) {
  if (setProperty(HasPendingChanges_, value, HasPendingChangesPropertyChangedEventArgs_) && value) {
    invokeCallback();
  }
}

bool AbsoluteMoveAndResizeViewModel::applyChanges() {
  if (!HasPendingChanges_) {
    return false;
  }

  if (X_dirty_) {
    X_dirty_ = false;
    action_.setX(X_);
  }

  if (Y_dirty_) {
    Y_dirty_ = false;
    action_.setY(Y_);
  }

  if (Width_dirty_) {
    Width_dirty_ = false;
    action_.setWidth(Width_);
  }

  if (Height_dirty_) {
    Height_dirty_ = false;
    action_.setHeight(Height_);
  }

  HasPendingChanges(false);
  return true;
}

void AbsoluteMoveAndResizeViewModel::X(int32_t value) {
  if (value < kAbsoluteCoordinateMin || kAbsoluteCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(X_, value, XPropertyChangedEventArgs_)) {
    X_dirty_ = true;
    HasPendingChanges(true);
  }
}

void AbsoluteMoveAndResizeViewModel::Y(int32_t value) {
  if (value < kAbsoluteCoordinateMin || kAbsoluteCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Y_, value, YPropertyChangedEventArgs_)) {
    Y_dirty_ = true;
    HasPendingChanges(true);
  }
}

void AbsoluteMoveAndResizeViewModel::Width(int32_t value) {
  if (value < kAbsoluteLengthMin || kAbsoluteLengthMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Width_, value, WidthPropertyChangedEventArgs_)) {
    Width_dirty_ = true;
    HasPendingChanges(true);
  }
}

void AbsoluteMoveAndResizeViewModel::Height(int32_t value) {
  if (value < kAbsoluteLengthMin || kAbsoluteLengthMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Height_, value, HeightPropertyChangedEventArgs_)) {
    Height_dirty_ = true;
    HasPendingChanges(true);
  }
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
