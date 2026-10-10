#include "pch.h"
#include "MoveAndResizeAction.h"

#include "../constants.h"
#include "../NodeHelper.h"

using namespace roxyg::settings::action;

AbsoluteMoveAndResizeAction::AbsoluteMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , window_bounds_(numeric::long4::make(
    ReadLongFromNode(node_[key::kPosX], 0, 0x4000),
    ReadLongFromNode(node_[key::kPosY], 0, 0x4000),
    ReadLongFromNode(node_[key::kWidth], 0, 0x4000),
    ReadLongFromNode(node_[key::kHeight], 0, 0x4000)
  )) {
}

RelativeMoveAndResizeAction::RelativeMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , name_(ReadStringAndMatchTypeFromNode(node_[key::kMonitorName]))
  , window_bounds_(numeric::float4::make(
    ReadFloatFromNodeOrDefault(node_[key::kPosX], 0.f, 1.f, 0.5f),
    ReadFloatFromNodeOrDefault(node_[key::kPosY], 0.f, 1.f, 0.5f),
    ReadFloatFromNodeOrDefault(node_[key::kWidth], 0.f, 1.f, 1.f),
    ReadFloatFromNodeOrDefault(node_[key::kHeight], 0.f, 1.f, 1.f)
  ))
  , id_(ReadIntFromNodeOrDefault(node_[key::kMonitorId], 1, 16, 0)) {
}
