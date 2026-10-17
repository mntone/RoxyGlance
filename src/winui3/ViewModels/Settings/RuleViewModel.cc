#include "pch.h"
#include "RuleViewModel.h"
#if __has_include("ViewModels/Settings/RuleViewModel.g.cpp")
#include "ViewModels/Settings/RuleViewModel.g.cpp"
#endif

// Filters
#include "FilterKeyViewModel.h"
// StringFilterViewModel is included by RuleViewModel.h.

// Actions
#include "ActionViewModel.h"
#include "AbsoluteMoveAndResizeViewModel.h"
#include "RelativeMoveAndResizeViewModel.h"

// Common
#include <ranges>
#include "SettingsWinRTConverters.h"

#include "../../../app/settings/action/MoveAndResize.h"

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

static inline winrt::ActionViewModel makeActionViewModel(Action& action) {
  using SAT = ::roxyg::settings::ActionType;

  winrt::ActionViewModel action_viewmodel{nullptr};
  switch (action.type()) {
  case SAT::kAbsoluteMoveAndResize:
    action_viewmodel = make<AbsoluteMoveAndResizeViewModel>(action.as<AbsoluteMoveAndResize>());
    break;
  case SAT::kRelativeMoveAndResize:
    action_viewmodel = make<RelativeMoveAndResizeViewModel>(action.as<RelativeMoveAndResize>());
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

RuleViewModel::RuleViewModel(Rule& rule)
  : StaticCommandHolder(&RuleViewModel::onAddFilterRequested)
  , rule_(rule)
  , Action_(makeActionViewModel(rule.action()))
  , Filters_(nullptr)
  , UnusedFilterKeys_(nullptr) {
  std::vector<winrt::StringFilterViewModel> new_filters;
  new_filters.reserve(kFilterKeyCount);

  std::vector<winrt::FilterKeyViewModel> new_unused_filters;
  new_unused_filters.reserve(kFilterKeyCount);

  Filter& f{rule.filter()};
  for (FilterKey key : kAllFilterKey) {
    if (hasFilterForKey(f, key)) {
      new_filters.push_back(make<StringFilterViewModel>(f, key, &RuleViewModel::onDeleteRequested, *this));
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
  Action_ = makeActionViewModel(action);
  notifyPropertyChanged(ActionPropertyChangedEventArgs_);
  notifyPropertyChanged(ActionTypePropertyChangedEventArgs_);
}

void RuleViewModel::save() {
  if (!deleting_keys_.empty()) {
    Filter& f{rule_.filter()};
    for (FilterKey key : deleting_keys_) {
      clearSettingsStringFilterValue(f, key);
    }
    deleting_keys_.clear();
  }

  if (Action_) {
    get_self<ActionViewModel>(Action_)->save();
  }
  for (winrt::StringFilterViewModel const& vm : Filters_) {
    get_self<StringFilterViewModel>(vm)->save();
  }
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

  self.Filters_.Append(make<StringFilterViewModel>(self.rule_.filter(), key, &RuleViewModel::onDeleteRequested, self, true));
}

void RuleViewModel::onDeleteRequested(StringFilterViewModel const& sender, IInspectable const& context) {
  using namespace std::views;

  com_ptr<RuleViewModel> self{context.as<RuleViewModel>()};

  FilterKey const key{sender.Key()};
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
      return;
    }
  }
  throw hresult_invalid_argument();
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation
