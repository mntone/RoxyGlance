#include "pch.h"
#include "NodeHelper.h"

#include "constants.h"

#pragma warning(push)
#pragma warning(disable:4244)
#include <magic_enum/magic_enum_containers.hpp>  // requires containers::bitset
#pragma warning(pop)
#include <magic_enum/magic_enum_flags.hpp>  // requires enum_flags_contains

namespace {
inline constexpr auto kAllTriggerEntries = magic_enum::enum_entries<roxyg::settings::TriggerFlags>();
inline constexpr auto kAllTriggerFlags = magic_enum::enum_values<roxyg::settings::TriggerFlags>();
}

using namespace c4::yml;
using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE std::string to_string(c4::csubstr str) {
  return {str.data(), str.size()};
}

static ROXYG_ALWAYS_INLINE std::optional<TriggerFlags> to_trigger(c4::csubstr str) {
  return magic_enum::enum_cast<TriggerFlags>(
    std::string_view{str.data(), str.size()},
    magic_enum::case_insensitive
  );
}

template<typename Visitor>
static bool visitMatchingTriggers(NodeRef seq, TriggerFlags value, Visitor&& visitor) {
#ifdef _DEBUG
  assert(seq.is_seq());
#endif

  for (NodeRef child = seq.first_child(); !child.invalid();) {
    NodeRef const next = child.next_sibling();
    if (child.has_val()) {
      std::optional<TriggerFlags> const flag{to_trigger(child.val())};
      if (flag && flag.value() == value && !visitor(child)) {
        return false;
      }
    }
    child = next;
  }
  return true;
}

static ROXYG_ALWAYS_INLINE bool containsTriggerFlag(NodeRef seq, TriggerFlags value) noexcept {
  bool found = false;
  visitMatchingTriggers(seq, value, [&found](NodeRef) {
    found = true;
    return false;
  });
  return found;
}

static ROXYG_ALWAYS_INLINE NodeRef getOrCreateWhenNode(NodeRef n) {
  // When `when` is missing:
  // 1. Insert it before `where`, if present.
  // 2. Otherwise, insert it before `then`, if present.
  // 3. Otherwise, append it.

  NodeRef seq;
  if (!n.has_child(key::kWhen)) {
    NodeInit const init{KEYSEQ | FLOW_SL, key::kWhen};
    if (n.has_child(key::kWhere)) {
      seq = n.insert_child(init, n[key::kWhere].prev_sibling());
    } else if (n.has_child(key::kThen)) {
      seq = n.insert_child(init, n[key::kThen].prev_sibling());
    } else {
      seq = n.append_child(init);
    }
  } else {
    seq = n[key::kWhen];
  }
  return seq;
}

HRESULT ReadTriggerFlagsFromNode(ConstNodeRef n, TriggerFlags* value) {
  using namespace magic_enum::bitwise_operators;

#ifdef _DEBUG
  assert(value);
#endif

  if (!n.is_map()) {
    return E_INVALIDARG;
  }

  if (!n.has_child(key::kWhen)) {
    *value = TriggerFlags::kNone;
    return S_OK;
  }

  ConstNodeRef const when{n[key::kWhen]};
  if (when.is_seq()) {
    TriggerFlags ret = TriggerFlags::kNone;
    for (ConstNodeRef v : when.children()) {
      if (v.has_val()) {
        ret |= to_trigger(v.val()).value_or(TriggerFlags::kNone);
      }
    }
    *value = ret;
    return S_OK;
  } else if (when.is_keyval()) {
    *value = to_trigger(when.val()).value_or(TriggerFlags::kNone);
    return S_OK;
  } else {
    return E_INVALIDARG;
  }
}

HRESULT WriteTriggerFlagsToNode(NodeRef n, TriggerFlags value) try {
  ROXYG_UNCHECKED_ASSERT(value == TriggerFlags::kNone || magic_enum::enum_flags_contains(value));

  if (!n.is_map()) {
    return E_INVALIDARG;
  }

  auto const bs = magic_enum::containers::bitset(value);

  NodeRef when = getOrCreateWhenNode(n);
#ifdef _DEBUG
  assert(when.is_seq() && when.has_key() || when.is_keyval());
#endif

  // Convert scalar values to a sequence before updating.
  bool const is_scalar = !when.is_seq();
  if (is_scalar) {
    std::string old_value = to_string(when.val());
    when.clear_val();
    when.set_type(KEYSEQ | FLOW_SL);
    when.append_child() << old_value;
  }

  // Remove triggers that are not requested.
  for (TriggerFlags trigger : kAllTriggerFlags) {
    if (bs.test(trigger)) {
      continue;
    }

    visitMatchingTriggers(when, trigger, [&when](NodeRef n) {
      when.remove_child(n);
      return true;
    });
  }

  // Append newly enabled triggers.
  for (auto const& [trigger, name_view] : kAllTriggerEntries) {
    if (!bs.test(trigger)) {
      continue;
    }

    if (!containsTriggerFlag(when, trigger)) {
      when.append_child() << c4::to_csubstr(name_view);
    }
  }

  // Preserve scalar form when the result contains a single trigger.
  if (is_scalar && when.num_children() == 1) {
    std::string old_value = to_string(when.first_child().val());
    when.clear_children();
    when.set_type(KEYVAL);
    when << c4::to_csubstr(old_value);
  }

  return S_OK;
} catch (std::bad_alloc const&) {
  return E_OUTOFMEMORY;
}
