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

static ROXYG_ALWAYS_INLINE std::optional<TriggerFlags> toOptionalTriggerFlags(c4::csubstr str) noexcept {
  return magic_enum::enum_cast<TriggerFlags>(
    std::string_view{str.data(), str.size()},
    magic_enum::case_insensitive
  );
}

static ROXYG_ALWAYS_INLINE TriggerFlags toTriggerFlags(c4::csubstr str) noexcept {
  return toOptionalTriggerFlags(str).value_or(TriggerFlags::kNone);
}

template<typename Visitor>
static bool visitMatchingTriggers(NodeRef seq, TriggerFlags value, Visitor&& visitor)
  noexcept(noexcept(std::declval<Visitor&>()(std::declval<NodeRef&>()))) {
#ifdef _DEBUG
  assert(seq.is_seq());
#endif

  for (NodeRef child = seq.first_child(); !child.invalid();) {
    NodeRef const next = child.next_sibling();
    if (child.has_val()) {
      std::optional<TriggerFlags> const flag{toOptionalTriggerFlags(child.val())};
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
  visitMatchingTriggers(seq, value, [&found](NodeRef) noexcept {
    found = true;
    return false;
  });
  return found;
}

TriggerFlags roxyg::settings::ReadTriggerFlagsFromNode(ConstNodeRef n) {
  using namespace magic_enum::bitwise_operators;

  if (!n.is_map()) {
    throw ParseError{ParseErrorReason::kExpectedMap, KeyId::kRule, 0};
  }

  if (!n.has_child(key::kWhen)) {
    return TriggerFlags::kNone;
  }

  ConstNodeRef const when{n[key::kWhen]};
  if (when.is_seq()) {
    TriggerFlags ret = TriggerFlags::kNone;
    for (ConstNodeRef v : when.children()) {
      if (v.has_val()) {
        ret |= toTriggerFlags(v.val());
      }
    }
    return ret;
  } else if (when.is_keyval()) {
    return toTriggerFlags(when.val());
  } else {
    throw ParseError{ParseErrorReason::kExpectedSequence, KeyId::kWhen, 0};
  }
}

HRESULT WriteTriggerFlagsToNode(NodeRef n, TriggerFlags value) try {
  ROXYG_UNCHECKED_ASSERT(value == TriggerFlags::kNone || magic_enum::enum_flags_contains(value));

#ifdef _DEBUG
  assert(n.is_map());  // Callers pass a node already validated by Rule.
#endif

  auto const bs = magic_enum::containers::bitset(value);

  NodeRef when;
  if (!n.has_child(key::kWhen)) {
#ifdef _DEBUG
    // The where key is required.
    assert(n.has_child(key::kWhere));
#endif

    NodeInit const init{KEYSEQ | FLOW_SL, key::kWhen};
    when = n.insert_child(init, n[key::kWhere].prev_sibling());
  } else {
    when = n[key::kWhen];
  }
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

    visitMatchingTriggers(when, trigger, [&when](NodeRef n) noexcept {
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
