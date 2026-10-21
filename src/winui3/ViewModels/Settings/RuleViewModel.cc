#include "pch.h"
#include "RuleViewModel.h"
#if __has_include("ViewModels/Settings/RuleViewModel.g.cpp")
#include "ViewModels/Settings/RuleViewModel.g.cpp"
#endif

// Triggers
// TriggersViewModel is included by RuleViewModel.h.

// Filters
#include "FilterKeyViewModel.h"
#include "StringFilterViewModel.h"

// Actions
// ActionViewModel is included by RuleViewModel.h.
#include "AbsoluteMoveAndResizeViewModel.h"
#include "RelativeMoveAndResizeViewModel.h"

// Common
#include <ranges>
#include "SettingsWinRTConverters.h"
#include "../../Resources/SettingsResource.h"

#include "../../../app/settings/action/MoveAndResize.h"  // requires Absolute/RelativeMoveAndResize
#include "../../../app/win32/string.h"                   // requires FormatResourceString

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;

using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Data;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

using namespace roxyg::settings;
using namespace roxyg::settings::action;
using namespace roxyg::winui3::viewmodel::settings;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

static inline hstring makeName(Rule const& rule) {
  std::wstring_view name{rule.name()};
  if (name.empty()) {
    return roxyg::win32::FormatResourceString(
      roxyg::winui3::string::kSettingsRuleFormat,
      rule.index()
    );
  }

  return hstring{name};
}

static inline com_ptr<ActionViewModel> makeActionViewModel(
  Action& action,
  AbsoluteMoveAndResizeViewModel::CallbackFunction callback,
  IInspectable const& context,
  bool change_type = false
) {
  using SAT = ::roxyg::settings::ActionType;

  com_ptr<ActionViewModel> action_viewmodel{nullptr};
  switch (action.type()) {
  case SAT::kAbsoluteMoveAndResize:
    action_viewmodel = make_self<AbsoluteMoveAndResizeViewModel>(
      action.as<AbsoluteMoveAndResize>(),
      callback, context, change_type
    );
    break;
  case SAT::kRelativeMoveAndResize:
    action_viewmodel = make_self<RelativeMoveAndResizeViewModel>(
      action.as<RelativeMoveAndResize>(),
      callback, context, change_type
    );
    break;
  case SAT::kInvalid:
    action_viewmodel = nullptr;
    break;
  default:
#ifdef _DEBUG
    __debugbreak();
#endif
    throw hresult_invalid_argument();
  }
  return action_viewmodel;
}

PropertyChangedEventArgs RuleViewModel::ActionPropertyChangedEventArgs_{nullptr};
PropertyChangedEventArgs RuleViewModel::ActionTypePropertyChangedEventArgs_{nullptr};

void RuleViewModel::initialize() {
  ActionPropertyChangedEventArgs_ = PropertyChangedEventArgs{L"Action"};
  ActionTypePropertyChangedEventArgs_ = PropertyChangedEventArgs{L"ActionType"};
}

RuleViewModel::RuleViewModel(
  Rule& rule,
  CallbackFunction callback,
  IInspectable const& context
)
  : CallbackHolder(callback, context)
  , StaticCommandHolder(&RuleViewModel::onAddFilterRequested)
  , rule_(rule)
  , Name_(makeName(rule))
  , Action_(makeActionViewModel(rule.action(), &RuleViewModel::onChildHasPendingChangesUpdated, *this))
  , Filters_(nullptr)
  , Triggers_(make_self<TriggersViewModel>(rule, &RuleViewModel::onChildHasPendingChangesUpdated, *this))
  , UnusedFilterKeys_(nullptr) {
  std::vector<winrt::StringFilterViewModel> new_filters;
  new_filters.reserve(kFilterKeyCount);

  std::vector<winrt::FilterKeyViewModel> new_unused_filters;
  new_unused_filters.reserve(kFilterKeyCount);

  Filter& f{rule.filter()};
  for (FilterKey key : kAllFilterKey) {
    if (hasFilterForKey(f, key)) {
      new_filters.push_back(make<StringFilterViewModel>(
        f,
        key,
        &RuleViewModel::onChildHasPendingChangesUpdated,
        &RuleViewModel::onDeleteRequested,
        *this
      ));
    } else {
      new_unused_filters.push_back(make<FilterKeyViewModel>(key, /*ICommand*/ *this));
    }
  }
  Filters_ = single_threaded_observable_vector<winrt::StringFilterViewModel>(std::move(new_filters));
  UnusedFilterKeys_ = single_threaded_observable_vector<winrt::FilterKeyViewModel>(std::move(new_unused_filters));
}

void RuleViewModel::ActionType(Settings::ActionType value) {
  roxyg::settings::ActionType const settings_value{toSettingsActionType(value)};
  roxyg::settings::Action& action{rule_.action()};
  if (action.type() == settings_value) {
    return;
  }

  action.setType(settings_value);
  Action_ = makeActionViewModel(action, &RuleViewModel::onChildHasPendingChangesUpdated, *this, true);
  notifyPropertyChanged(ActionPropertyChangedEventArgs_);
  notifyPropertyChanged(ActionTypePropertyChangedEventArgs_);
  notifyHasPendingChangesPropertyChanged();
}

bool RuleViewModel::HasPendingChanges() const noexcept {
  if (!deleting_keys_.empty()) {
    return true;
  }

  com_ptr<ActionViewModel> const action{Action_};
  if (action && action->HasPendingChanges()) {
    return true;
  }
  for (winrt::StringFilterViewModel const& filter : Filters_) {
    if (filter.HasPendingChanges()) {
      return true;
    }
  }
  return Triggers_->HasPendingChanges();
}

bool RuleViewModel::applyChanges() {
  bool changed = false;

  if (!deleting_keys_.empty()) {
    Filter& f{rule_.filter()};
    for (FilterKey key : deleting_keys_) {
      clearSettingsStringFilterValue(f, key);
    }
    deleting_keys_.clear();

    changed = true;
  }

  com_ptr<ActionViewModel> const action{Action_};
  if (action && action->applyChanges()) {
    changed = true;
  }
  for (winrt::StringFilterViewModel const& filter : Filters_) {
    if (get_self<StringFilterViewModel>(filter)->applyChanges()) {
      changed = true;
    }
  }
  if (Triggers_->applyChanges()) {
    changed = true;
  }

  if (changed) {
    notifyHasPendingChangesPropertyChanged();
  }
  return changed;
}

void RuleViewModel::notifyHasPendingChangesPropertyChanged() {
  notifyPropertyChanged(HasPendingChangesPropertyChangedEventArgs_);
  invokeCallback();
}

void RuleViewModel::onAddFilterRequested(RuleViewModel& self, IInspectable const& param) {
  FilterKey const key{unbox_value<FilterKey>(param)};

  uint32_t target_index{UINT32_MAX};
  for (winrt::FilterKeyViewModel const& vm : self.UnusedFilterKeys_) {
    if (vm.Key() == key) {
      if (!self.UnusedFilterKeys_.IndexOf(vm, target_index)) {
        throw hresult_invalid_argument();
      }
      break;
    }
  }
  if (target_index == UINT32_MAX) {
    throw hresult_invalid_argument();
  }
  self.UnusedFilterKeys_.RemoveAt(target_index);

  self.Filters_.Append(make<StringFilterViewModel>(
    self.rule_.filter(),
    key,
    &RuleViewModel::onChildHasPendingChangesUpdated,
    &RuleViewModel::onDeleteRequested,
    self,
    true
  ));

  self.notifyHasPendingChangesPropertyChanged();
}

void RuleViewModel::onChildHasPendingChangesUpdated(IInspectable const& context) {
  com_ptr<RuleViewModel> self{context.as<RuleViewModel>()};
  self->notifyHasPendingChangesPropertyChanged();
}

void RuleViewModel::onDeleteRequested(IInspectable const& context, FilterKey key) {
  using namespace std::views;

  com_ptr<RuleViewModel> self{context.as<RuleViewModel>()};
  for (auto const& [index, vm] : zip(iota(uint32_t{0}), self->Filters_)) {
    if (vm.Key() == key) {
      self->Filters_.RemoveAt(index);
      self->deleting_keys_.emplace_back(key);

      uint32_t insertion_index{0};
      bool key_found{false};
      for (FilterKey ordered_key : kAllFilterKey) {
        if (ordered_key == key) {
          key_found = true;
          break;
        }
        for (winrt::FilterKeyViewModel const& unused_key : self->UnusedFilterKeys_) {
          if (unused_key.Key() == ordered_key) {
            ++insertion_index;
            break;
          }
        }
      }
      if (!key_found) {
        throw hresult_invalid_argument();
      }
      self->UnusedFilterKeys_.InsertAt(insertion_index, make<FilterKeyViewModel>(key, *self));
      self->notifyHasPendingChangesPropertyChanged();
      return;
    }
  }
  throw hresult_invalid_argument();
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
