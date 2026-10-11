#include "pch.h"
#include "Rule.h"

#include "constants.h"
#include "NodeHelper.h"

using namespace c4::yml;
using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE void checkRuleNode(ConstNodeRef n) {
  if (!n.is_map()) {
    throw ParseError{ParseErrorReason::kExpectedMap, KeyId::kRule, 0};
  }

  if (!n.has_child(key::kWhere)) {
    throw ParseError{ParseErrorReason::kMissingRequiredKey, KeyId::kWhere, 0};
  }

  if (!n.has_child(key::kThen)) {
    throw ParseError{ParseErrorReason::kMissingRequiredKey, KeyId::kThen, 0};
  }
}

Rule::Rule(NodeRef node)
  // Validate through a const view before taking ownership of the node.
  : node_((checkRuleNode(node), std::move(node)))
  , name_(ReadStringFromNode(node_[key::kName], KeyId::kName))
  , filter_(node_[key::kWhere])
  , triggers_(ReadTriggerFlagsFromNode(node_))
  , action_(node_[key::kThen]) {
#ifdef _DEBUG
  assert(node_.is_map());
#endif
}

void Rule::setTriggerFlags(TriggerFlags value) {
  if (triggers_ == value) {
    return;
  }

  winrt::hresult const hr = WriteTriggerFlagsToNode(node_, value);
  if (FAILED(hr)) {
    winrt::throw_hresult(hr);
  }

  triggers_ = value;
}
