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

HRESULT ReadActionTypeFromNode(ConstNodeRef n, ActionType* value) {
#ifdef _DEBUG
  assert(value);
#endif

  if (!n.is_map()) {
    return E_INVALIDARG;
  }

  if (!n.has_child(key::kType)) {
    *value = ActionType::kInvalid;
    return S_OK;
  }

  ConstNodeRef const type{n[key::kType]};
  if (!type.is_keyval()) {
    return E_INVALIDARG;
  }

  *value = toActionType(type.val());
  return S_OK;
}

HRESULT WriteActionTypeToNode(NodeRef n, ActionType value) {
  if (!n.is_map()) {
    return E_INVALIDARG;
  }

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

static ROXYG_ALWAYS_INLINE ActionType readActionType(c4::yml::ConstNodeRef n) {
  ActionType value;
  HRESULT const hr = ReadActionTypeFromNode(n, &value);
  winrt::check_hresult(hr);
  return value;
}

Action::Action(c4::yml::NodeRef node)
  : node_(std::move(node))
  , type_(readActionType(node_)) {
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
