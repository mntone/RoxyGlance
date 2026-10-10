#include "pch.h"
#include "MoveAndResizeAction.h"

#include "../constants.h"
#include "../NodeHelper.h"

using namespace roxyg::settings::action;

AbsoluteMoveAndResizeAction::AbsoluteMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , window_bounds_(numeric::long4::make(
    ReadLongFromNode(node_[key::kPosX], numeric_limit::kAbsoluteCoordinateMin, numeric_limit::kAbsoluteCoordinateMax),
    ReadLongFromNode(node_[key::kPosY], numeric_limit::kAbsoluteCoordinateMin, numeric_limit::kAbsoluteCoordinateMax),
    ReadLongFromNode(node_[key::kWidth], numeric_limit::kAbsoluteCoordinateMin, numeric_limit::kAbsoluteCoordinateMax),
    ReadLongFromNode(node_[key::kHeight], numeric_limit::kAbsoluteCoordinateMin, numeric_limit::kAbsoluteCoordinateMax)
  )) {
}

void AbsoluteMoveAndResizeAction::setX(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.x() != val) {
    WriteLongToNode(node_[key::kPosX], val);
    window_bounds_.setX(val);
  }
}

void AbsoluteMoveAndResizeAction::setY(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.y() != val) {
    WriteLongToNode(node_[key::kPosY], val);
    window_bounds_.setY(val);
  }
}

void AbsoluteMoveAndResizeAction::setWidth(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.z() != val) {
    WriteLongToNode(node_[key::kWidth], val);
    window_bounds_.setZ(val);
  }
}

void AbsoluteMoveAndResizeAction::setHeight(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.w() != val) {
    WriteLongToNode(node_[key::kHeight], val);
    window_bounds_.setW(val);
  }
}

RelativeMoveAndResizeAction::RelativeMoveAndResizeAction(c4::yml::NodeRef node)
  : node_(std::move(node))
  , name_(ReadStringAndMatchTypeFromNode(node_[key::kMonitorName]))
  , window_bounds_(numeric::float4::make(
    ReadFloatFromNodeOrDefault(
      node_[key::kPosX],
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      0.5f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kPosY],
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      0.5f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kWidth],
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      1.f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kHeight],
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      1.f
    )
  ))
  , id_(ReadIntFromNodeOrDefault(
    node_[key::kMonitorId],
    numeric_limit::kRelativeMonitorIdMin,
    numeric_limit::kRelativeMonitorIdMax,
    numeric_limit::kRelativeMonitorIdUnset
  )) {
}

void RelativeMoveAndResizeAction::setId(int val) {
  if (val == numeric_limit::kRelativeMonitorIdUnset) {
    if (node_.has_child(key::kMonitorId)) {
      c4::yml::NodeRef child{node_[key::kMonitorId]};
      node_.remove_child(child);
    }
    id_ = val;
    return;
  }

  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeMonitorIdMin <= val
    && val <= numeric_limit::kRelativeMonitorIdMax
  );

  if (id_ != val) {
    WriteIntToNode(node_[key::kMonitorId], val);
    id_ = val;
  }
}

void RelativeMoveAndResizeAction::setName(StringAndMatchType val) {
  if (name_ != val) {
    WriteStringAndMatchTypeToNode(node_[key::kMonitorName], val);
    name_ = val;
  }
}

void RelativeMoveAndResizeAction::setX(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.x() != val) {
    WriteFloatToNode(node_[key::kPosX], val);
    window_bounds_.setX(val);
  }
}

void RelativeMoveAndResizeAction::setY(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.y() != val) {
    WriteFloatToNode(node_[key::kPosY], val);
    window_bounds_.setY(val);
  }
}

void RelativeMoveAndResizeAction::setWidth(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.z() != val) {
    WriteFloatToNode(node_[key::kWidth], val);
    window_bounds_.setZ(val);
  }
}

void RelativeMoveAndResizeAction::setHeight(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.w() != val) {
    WriteFloatToNode(node_[key::kHeight], val);
    window_bounds_.setW(val);
  }
}
