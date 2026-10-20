#include "pch.h"
#include "RelativeMoveAndResizeViewModel.h"
#if __has_include("ViewModels/Settings/RelativeMoveAndResizeViewModel.g.cpp")
#include "ViewModels/Settings/RelativeMoveAndResizeViewModel.g.cpp"
#endif

#include "../../../app/settings/constants.h"

namespace winrt {
using namespace ::winrt::Microsoft::UI::Xaml::Data;
}

using namespace roxyg::settings::action;
using namespace roxyg::settings::numeric_limit;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

PropertyChangedEventArgs RelativeMoveAndResizeViewModel::IdPropertyChangedEventArgs_{nullptr};

void RelativeMoveAndResizeViewModel::initialize() {
  IdPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Id"};
}

RelativeMoveAndResizeViewModel::RelativeMoveAndResizeViewModel(
  RelativeMoveAndResize action,
  CallbackFunction callback,
  IInspectable const& context,
  bool change_type
)
  : base_type(ActionType::RelativeMoveAndResize)
  , CallbackHolder(callback, context)
  , action_(std::move(action))
  , X_(action.windowBounds().x())
  , Y_(action.windowBounds().y())
  , Width_(action.windowBounds().z())
  , Height_(action.windowBounds().w())
  //, Name_(action.name())
  , Id_(action.id())
  , HasPendingChanges_(change_type)
  , Id_dirty_(false)
  //, Name_dirty_(false)
  , X_dirty_(false)
  , Y_dirty_(false)
  , Width_dirty_(false)
  , Height_dirty_(false) {
}

bool RelativeMoveAndResizeViewModel::HasPendingChanges() const noexcept {
  return HasPendingChanges_;
}

void RelativeMoveAndResizeViewModel::HasPendingChanges(bool value) {
  if (setProperty(HasPendingChanges_, value, HasPendingChangesPropertyChangedEventArgs_) && value) {
    invokeCallback();
  }
}

void RelativeMoveAndResizeViewModel::Id(int value) {
  if (value != kRelativeMonitorIdUnset
    && (value < kRelativeMonitorIdMin || kRelativeMonitorIdMax < value)) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Id_, value, IdPropertyChangedEventArgs_)) {
    Id_dirty_ = true;
    HasPendingChanges(true);
  }
}

//void RelativeMoveAndResizeViewModel::Name(hstring value) {
//  if (setProperty(Name_, value, L"Name")) {
//    Name_dirty_ = true;
//  }
//}

void RelativeMoveAndResizeViewModel::X(float value) {
#ifndef _M_FP_FAST
  if (!std::isfinite(value)) {
    throw hresult_invalid_argument();
  }
#endif
  if (value < kRelativeCoordinateMin || kRelativeCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(X_, value, XPropertyChangedEventArgs_)) {
    X_dirty_ = true;
    HasPendingChanges(true);
  }
}

void RelativeMoveAndResizeViewModel::Y(float value) {
#ifndef _M_FP_FAST
  if (!std::isfinite(value)) {
    throw hresult_invalid_argument();
  }
#endif
  if (value < kRelativeCoordinateMin || kRelativeCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Y_, value, YPropertyChangedEventArgs_)) {
    Y_dirty_ = true;
    HasPendingChanges(true);
  }
}

void RelativeMoveAndResizeViewModel::Width(float value) {
#ifndef _M_FP_FAST
  if (!std::isfinite(value)) {
    throw hresult_invalid_argument();
  }
#endif
  if (value < kRelativeCoordinateMin || kRelativeCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Width_, value, WidthPropertyChangedEventArgs_)) {
    Width_dirty_ = true;
    HasPendingChanges(true);
  }
}

void RelativeMoveAndResizeViewModel::Height(float value) {
#ifndef _M_FP_FAST
  if (!std::isfinite(value)) {
    throw hresult_invalid_argument();
  }
#endif
  if (value < kRelativeCoordinateMin || kRelativeCoordinateMax < value) {
    throw hresult_invalid_argument();
  }

  if (setProperty(Height_, value, HeightPropertyChangedEventArgs_)) {
    Height_dirty_ = true;
    HasPendingChanges(true);
  }
}

bool RelativeMoveAndResizeViewModel::applyChanges() {
  if (!HasPendingChanges_) {
    return false;
  }

  if (Id_dirty_) {
    Id_dirty_ = false;
    action_.setId(Id_);
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

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
