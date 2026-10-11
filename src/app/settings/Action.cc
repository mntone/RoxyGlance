#include "pch.h"
#include "constants.h"
#include "NodeHelper.h"

using namespace c4::yml;
using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE ActionType toActionType(c4::csubstr str) {
  return magic_enum::enum_cast<ActionType>(
    std::string_view{str.data(), str.size()},
    magic_enum::case_insensitive
  ).value_or(ActionType::kInvalid);
}

ActionType roxyg::settings::ReadActionTypeFromNode(ConstNodeRef n) {
  if (!n.has_child(key::kType)) {
    return ActionType::kInvalid;
  }

  ConstNodeRef const type{n[key::kType]};
  if (!type.is_keyval()) {
    throw ParseError{ParseErrorReason::kExpectedString, KeyId::kActionType, 0};
  }

  return toActionType(type.val());
}

HRESULT WriteActionTypeToNode(NodeRef n, ActionType value) {
#ifdef _DEBUG
  assert(n.is_map());  // Callers pass a node already validated by Action.
#endif

  if (value == ActionType::kInvalid) {
    if (n.has_child(key::kType)) {
      NodeRef type{n[key::kType]};
      n.remove_child(type);
    }
    return S_OK;
  }

  NodeRef type;
  if (!n.has_child(key::kType)) {
    NodeInit const init{KEYVAL, key::kType};
    type = n.append_child(init);
  } else {
    type = n[key::kType];
#ifdef _DEBUG
    assert(type.is_keyval());
#endif
  }

  std::string_view const value_name{magic_enum::enum_name(value)};
  ROXYG_UNCHECKED_ASSERT(!value_name.empty());
  type << c4::to_csubstr(value_name);
  return S_OK;
}

static ROXYG_ALWAYS_INLINE void checkActionNode(ConstNodeRef n) {
  if (!n.is_map()) {
    throw ParseError{ParseErrorReason::kExpectedMap, KeyId::kThen, 0};
  }
}

Action::Action(c4::yml::NodeRef node)
  // Validate through a const view before taking ownership of the node.
  : node_((checkActionNode(node), std::move(node)))
  , type_(ReadActionTypeFromNode(node_)) {
}

void Action::setType(ActionType value) {
  if (type_ == value) {
    return;
  }

  winrt::hresult const hr = WriteActionTypeToNode(node_, value);
  if (FAILED(hr)) {
    winrt::throw_hresult(hr);
  }

  type_ = value;
}
