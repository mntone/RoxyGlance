#include "pch.h"
#include "RuleViewModel.h"
#if __has_include("ViewModels/Settings/RuleViewModel.g.cpp")
#include "ViewModels/Settings/RuleViewModel.g.cpp"
#endif

// Filters
#include "FilterKeyViewModel.h"
// StringFilterViewModel is included by RuleViewModel.h.

// Common
#include <ranges>
#include "SettingsWinRTConverters.h"

#include "../../../app/settings/action/MoveAndResize.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Settings;
}

using namespace roxyg::settings;
using namespace roxyg::settings::action;
using namespace roxyg::winui3::viewmodel::settings;

namespace winrt::Mntone::RoxyGlance::ViewModels::Settings::implementation {

RuleViewModel::RuleViewModel(Rule& rule)
  : StaticCommandHolder(&RuleViewModel::onAddFilterRequested)
  , rule_(rule)
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

void RuleViewModel::save() {
  if (!deleting_keys_.empty()) {
    Filter& f{rule_.filter()};
    for (FilterKey key : deleting_keys_) {
      clearSettingsStringFilterValue(f, key);
    }
    deleting_keys_.clear();
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
