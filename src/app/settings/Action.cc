#include "pch.h"
#include "Action.h"

#include "constants.h"
#include "NodeHelper.h"

namespace magic_enum::customize {

template<>
constexpr customize_t enum_name(roxyg::settings::ActionType value) noexcept {
  using AT = roxyg::settings::ActionType;

  switch (value) {
  case AT::kAbsoluteMoveAndResize:
    return "absolute_move_and_resize";
  case AT::kRelativeMoveAndResize:
    return "relative_move_and_resize";
  default:
    return invalid_tag;
  }
}

}  // namespace magic_enum::customize

using namespace magic_enum;
using namespace roxyg::settings;

static __forceinline ActionType readActionType(c4::yml::ConstNodeRef n) {
  std::string const action_type = ReadStringFromNode(n);
  return enum_cast<ActionType>(action_type, case_insensitive)
    .value_or(ActionType::kUnknown);
}

Action::Action(c4::yml::NodeRef node)
  : node_(std::move(node))
  , type_(readActionType(node_[key::kType])) {
}
