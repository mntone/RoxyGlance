#include "pch.h"
#include "CompileFilterPrivate.h"

#include "../../window/WindowState.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

static bool matchesAlwaysFalse([[maybe_unused]] Predicate::DataType const& data, [[maybe_unused]] window::State& windowState) {
  return false;
}

static bool matchesWindowTitleEquals(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowTitle() == std::get<std::wstring>(data);
}

static bool matchesWindowTitleContains(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowTitle().contains(std::get<std::wstring>(data));
}

static bool matchesWindowTitleStartsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowTitle().starts_with(std::get<std::wstring>(data));
}

static bool matchesWindowTitleEndsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.windowTitle().ends_with(std::get<std::wstring>(data));
}

Predicate filter::CompileWindowTitle(settings::StringAndCompareType const& conf) {
  using CT = settings::StringCompareType;

  Predicate::FunctionType fn;
  switch (conf.first) {
  case CT::kEquals:
    fn = matchesWindowTitleEquals;
    break;
  case CT::kContains:
    fn = matchesWindowTitleContains;
    break;
  case CT::kStartsWith:
    fn = matchesWindowTitleStartsWith;
    break;
  case CT::kEndsWith:
    fn = matchesWindowTitleEndsWith;
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
