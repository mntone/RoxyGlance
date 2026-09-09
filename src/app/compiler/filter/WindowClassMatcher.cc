#include "pch.h"
#include "CompileFilterPrivate.h"

#include "../../window/WindowState.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

static bool matchesAlwaysFalse([[maybe_unused]] Predicate::DataType const& data, [[maybe_unused]] window::State& windowState) {
  return false;
}

static bool matchesWindowClassEquals(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowClass() == std::get<std::wstring>(data);
}

static bool matchesWindowClassContains(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowClass().contains(std::get<std::wstring>(data));
}

static bool matchesWindowClassStartsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowClass().starts_with(std::get<std::wstring>(data));
}

static bool matchesWindowClassEndsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowClass().ends_with(std::get<std::wstring>(data));
}

Predicate filter::CompileWindowClass(settings::StringAndCompareType const& conf) {
  using CT = settings::StringCompareType;

  Predicate::FunctionType fn;
  switch (conf.first) {
  case CT::kEquals:
    fn = matchesWindowClassEquals;
    break;
  case CT::kContains:
    fn = matchesWindowClassContains;
    break;
  case CT::kStartsWith:
    fn = matchesWindowClassStartsWith;
    break;
  case CT::kEndsWith:
    fn = matchesWindowClassEndsWith;
    break;
  default:
    return {
      .evaluate = matchesAlwaysFalse,
    };
  }

  return {
    .evaluate = fn,
    .data = conf.second,
  };
}
