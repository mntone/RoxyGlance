#include "pch.h"
#include "CompileFilter.h"
#include "CompileFilterPrivate.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

PredicateSet filter::CompileFilter(settings::Filter const& filter) {
  PredicateSet condition;

  settings::StringAndCompareType const& windowClass = filter.windowClass();
  if (windowClass.first != settings::StringCompareType::kNone) {
    condition.emplace_back(filter::CompileWindowClass(windowClass));
  }

  settings::StringAndCompareType const& windowTitle = filter.windowTitle();
  if (windowTitle.first != settings::StringCompareType::kNone) {
    condition.emplace_back(filter::CompileWindowTitle(windowTitle));
  }

  return std::move(condition);
}
