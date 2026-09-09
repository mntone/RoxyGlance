#include "pch.h"
#include "CompileAction.h"

#include "../engine/MoveAndResizeOperation.h"
#include "../settings/action/MoveAndResizeAction.h"

using namespace roxyg;
using namespace roxyg::engine;

OperationSet compiler::CompileAction(settings::Actions const& actions) {
  using AT = settings::ActionType;

  OperationSet operations;
  for (auto const& action : actions) {
    switch (action.type()) {
    case AT::kAbsoluteMoveAndResize:
    {
      auto const& detail = settings::action::AbsoluteMoveAndResizeAction(action.node());
      operations.emplace_back(std::make_shared<AbsoluteMoveAndResizeOperation>(detail.windowBounds()));
    }
    case AT::kRelativeMoveAndResize:
    {
      auto const& detail = settings::action::RelativeMoveAndResizeAction(action.node());
      operations.emplace_back(std::make_shared<RelativeMoveAndResizeOperation>(detail.windowBounds()));
    }
    default:
      break;
    }
  }
  return operations;
}
