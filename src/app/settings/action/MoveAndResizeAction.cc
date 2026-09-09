#include "pch.h"
#include "MoveAndResizeAction.h"

#include "../constants.h"
#include "../NodeHelper.h"

using namespace roxyg::settings::action;

AbsoluteMoveAndResizeAction::AbsoluteMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , window_bounds_(numeric::long4::make(
    ReadBoundedLongFromNode(node_[key::kPosX], 0, 0x4000),
    ReadBoundedLongFromNode(node_[key::kPosY], 0, 0x4000),
    ReadBoundedLongFromNode(node_[key::kWidth], 0, 0x4000),
    ReadBoundedLongFromNode(node_[key::kHeight], 0, 0x4000)
  )) {
}

RelativeMoveAndResizeAction::RelativeMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , window_bounds_(numeric::float4::make(
    ReadBoundedFloatFromNodeOrDefault(node_[key::kPosX], 0.f, 1.f, 0.5f),
    ReadBoundedFloatFromNodeOrDefault(node_[key::kPosY], 0.f, 1.f, 0.5f),
    ReadBoundedFloatFromNodeOrDefault(node_[key::kWidth], 0.f, 1.f, 1.f),
    ReadBoundedFloatFromNodeOrDefault(node_[key::kHeight], 0.f, 1.f, 1.f)
  )) {
}
