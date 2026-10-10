#include "pch.h"
#include "CompileFilterPrivate.h"

#include "../../window/WindowState.h"

using namespace roxyg;
using namespace roxyg::compiler;
using namespace roxyg::engine;

static bool matchesAlwaysFalse([[maybe_unused]] Predicate::DataType const& data, [[maybe_unused]] window::State& windowState) {
  return false;
}

static bool matchesProcessImageNameEquals(Predicate::DataType const& data, window::State& windowState) {
  return windowState.processImageName() == std::get<std::wstring>(data);
}

static bool matchesProcessImageNameContains(Predicate::DataType const& data, window::State& windowState) {
  return windowState.processImageName().contains(std::get<std::wstring>(data));
}

static bool matchesProcessImageNameStartsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.processImageName().starts_with(std::get<std::wstring>(data));
}

static bool matchesProcessImageNameEndsWith(Predicate::DataType const& data, window::State& windowState) {
  return windowState.processImageName().ends_with(std::get<std::wstring>(data));
}

Predicate filter::CompileProcessImageName(settings::StringAndMatchType const& conf) {
  using MT = settings::StringMatchType;

  Predicate::FunctionType fn;
  switch (conf.first) {
  case MT::kEquals:
    fn = matchesProcessImageNameEquals;
    break;
  case MT::kContains:
    fn = matchesProcessImageNameContains;
    break;
  case MT::kStartsWith:
    fn = matchesProcessImageNameStartsWith;
    break;
  case MT::kEndsWith:
    fn = matchesProcessImageNameEndsWith;
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
