#include "pch.h"
#include "CompileFilter.h"
#include "CompileFilterPrivate.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

PredicateSet filter::CompileFilter(settings::Filter const& filter) {
  PredicateSet condition;

  settings::StringAndMatchType const& processImageName = filter.processImageName();
  if (!processImageName.second.empty()) {
    condition.emplace_back(filter::CompileProcessImageName(processImageName));
  }

  settings::StringAndMatchType const& windowClass = filter.windowClass();
  if (!windowClass.second.empty() || windowClass.first == settings::StringMatchType::kEquals) {
    condition.emplace_back(filter::CompileWindowClass(windowClass));
  }

  settings::StringAndMatchType const& windowTitle = filter.windowTitle();
  if (!windowTitle.second.empty() || windowTitle.first == settings::StringMatchType::kEquals) {
    condition.emplace_back(filter::CompileWindowTitle(windowTitle));
  }

  return std::move(condition);
}
