#include "pch.h"
#include "CompileAction.h"

#include "../engine/MoveAndResizeOperation.h"
#include "../settings/action/MoveAndResize.h"

using namespace roxyg;
using namespace roxyg::engine;
using namespace roxyg::settings::action;

engine::Operation compiler::CompileAction(CompilationContext& ctx, settings::Action const& action) {
  using AT = settings::ActionType;

  engine::Operation operation;
  switch (action.type()) {
  case AT::kAbsoluteMoveAndResize:
  {
    auto const detail = action.as<AbsoluteMoveAndResize>();
    operation = std::make_shared<AbsoluteMoveAndResizeOperation>(detail.windowBounds());
    break;
  }
  case AT::kRelativeMoveAndResize:
  {
    auto const detail = action.as<RelativeMoveAndResize>();
    HMONITOR const handle{ctx.resolveMonitorId(detail.id())};
    if (handle) {
      operation = std::make_shared<RelativeMoveAndResizeOperation>(
        handle,
        detail.windowBounds()
      );
    }
    break;
  }
  default:
    break;
  }
  return operation;
}
