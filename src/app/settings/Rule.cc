#include "pch.h"
#include "Rule.h"

#include "constants.h"
#include "NodeHelper.h"

using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE TriggerFlags readTriggers(c4::yml::ConstNodeRef n) {
  TriggerFlags value;
  HRESULT const hr = ReadTriggerFlagsFromNode(n, &value);
  winrt::check_hresult(hr);
  return value;
}

static ROXYG_ALWAYS_INLINE Filter readFilter(c4::yml::NodeRef n) {
  if (!n.has_child(key::kWhere)) {
    c4::yml::NodeRef target;
    if (n.has_child(key::kWhen)) {
      target = n.insert_child(n[key::kWhen]);
    } else if (n.has_child(key::kThen)) {
      target = n.insert_child(n[key::kThen].prev_sibling());
    } else {
      target = n.append_child();
    }

    c4::yml::NodeRef where = target << c4::yml::key(key::kWhere) << c4::yml::MAP;
    return Filter{ where };
  }

  return Filter{ n[key::kWhere] };
}

Rule::Rule(c4::yml::NodeRef node)
  : node_(std::move(node))
  , name_(ReadStringFromNode(node_[key::kName]))
  , filter_(readFilter(node_))
  , triggers_(readTriggers(node_))
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
