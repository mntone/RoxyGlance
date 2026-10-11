#include "pch.h"
#include "MoveAndResize.h"

#include "../constants.h"
#include "../NodeHelper.h"

using namespace roxyg::settings::action;

AbsoluteMoveAndResize::AbsoluteMoveAndResize(c4::yml::NodeRef node)
  : node_(std::move(node))
  , window_bounds_(numeric::long4::make(
    ReadLongFromNodeOrDefault(
      node_[key::kPosX],
      KeyId::kPositionX,
      numeric_limit::kAbsoluteCoordinateMin,
      numeric_limit::kAbsoluteCoordinateMax,
      0
    ),
    ReadLongFromNodeOrDefault(
      node_[key::kPosY],
      KeyId::kPositionY,
      numeric_limit::kAbsoluteCoordinateMin,
      numeric_limit::kAbsoluteCoordinateMax,
      0
    ),
    ReadLongFromNodeOrDefault(
      node_[key::kWidth],
      KeyId::kWidth,
      numeric_limit::kAbsoluteCoordinateMin,
      numeric_limit::kAbsoluteCoordinateMax,
      640
    ),
    ReadLongFromNodeOrDefault(
      node_[key::kHeight],
      KeyId::kHeight,
      numeric_limit::kAbsoluteCoordinateMin,
      numeric_limit::kAbsoluteCoordinateMax,
      400
    )
  )) {
}

void AbsoluteMoveAndResize::setX(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.x() != val) {
    WriteLongToNode(node_[key::kPosX], val);
    window_bounds_.setX(val);
  }
}

void AbsoluteMoveAndResize::setY(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.y() != val) {
    WriteLongToNode(node_[key::kPosY], val);
    window_bounds_.setY(val);
  }
}

void AbsoluteMoveAndResize::setWidth(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.z() != val) {
    WriteLongToNode(node_[key::kWidth], val);
    window_bounds_.setZ(val);
  }
}

void AbsoluteMoveAndResize::setHeight(long val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kAbsoluteCoordinateMin <= val
    && val <= numeric_limit::kAbsoluteCoordinateMax
  );

  if (window_bounds_.w() != val) {
    WriteLongToNode(node_[key::kHeight], val);
    window_bounds_.setW(val);
  }
}

RelativeMoveAndResize::RelativeMoveAndResize(c4::yml::NodeRef node)
  : node_(std::move(node))
  , name_(ReadStringAndMatchTypeFromNode(node_[key::kMonitorName], KeyId::kMonitorName))
  , window_bounds_(numeric::float4::make(
    ReadFloatFromNodeOrDefault(
      node_[key::kPosX],
      KeyId::kPositionX,
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      0.5f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kPosY],
      KeyId::kPositionY,
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      0.5f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kWidth],
      KeyId::kWidth,
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      1.f
    ),
    ReadFloatFromNodeOrDefault(
      node_[key::kHeight],
      KeyId::kHeight,
      numeric_limit::kRelativeCoordinateMin,
      numeric_limit::kRelativeCoordinateMax,
      1.f
    )
  ))
  , id_(ReadIntFromNodeOrDefault(
    node_[key::kMonitorId],
    KeyId::kMonitorId,
    numeric_limit::kRelativeMonitorIdMin,
    numeric_limit::kRelativeMonitorIdMax,
    numeric_limit::kRelativeMonitorIdUnset
  )) {
}

void RelativeMoveAndResize::setId(int val) {
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

void RelativeMoveAndResize::setName(StringAndMatchType val) {
  if (name_ != val) {
    WriteStringAndMatchTypeToNode(node_[key::kMonitorName], val);
    name_ = val;
  }
}

void RelativeMoveAndResize::setX(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.x() != val) {
    WriteFloatToNode(node_[key::kPosX], val);
    window_bounds_.setX(val);
  }
}

void RelativeMoveAndResize::setY(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.y() != val) {
    WriteFloatToNode(node_[key::kPosY], val);
    window_bounds_.setY(val);
  }
}

void RelativeMoveAndResize::setWidth(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.z() != val) {
    WriteFloatToNode(node_[key::kWidth], val);
    window_bounds_.setZ(val);
  }
}

void RelativeMoveAndResize::setHeight(float val) {
  ROXYG_UNCHECKED_ASSERT(
    numeric_limit::kRelativeCoordinateMin <= val
    && val <= numeric_limit::kRelativeCoordinateMax
  );

  if (window_bounds_.w() != val) {
    WriteFloatToNode(node_[key::kHeight], val);
    window_bounds_.setW(val);
  }
}
