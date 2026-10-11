#pragma once
#include "ParseError.h"
#include "Action.h"
#include "String.h"
#include "TriggerFlags.h"

namespace roxyg::settings {

long ReadLongFromNode(c4::yml::ConstNodeRef node, KeyId id, long min_val, long max_val);
long ReadLongFromNodeOrDefault(c4::yml::ConstNodeRef node, KeyId id, long min_val, long max_val, long def_val);

float ReadFloatFromNode(c4::yml::ConstNodeRef node, KeyId id, float min_val, float max_val);
float ReadFloatFromNodeOrDefault(c4::yml::ConstNodeRef node, KeyId id, float min_val, float max_val, float def_val);

std::wstring ReadStringFromNode(c4::yml::ConstNodeRef node, KeyId id);
StringAndMatchType ReadStringAndMatchTypeFromNode(c4::yml::ConstNodeRef node, KeyId id);

void WriteLongToNode(c4::yml::NodeRef node, long val);
void WriteFloatToNode(c4::yml::NodeRef node, float val);
void WriteStringAndMatchTypeToNode(c4::yml::NodeRef node, StringAndMatchType val);

ActionType ReadActionTypeFromNode(::c4::yml::ConstNodeRef n);
TriggerFlags ReadTriggerFlagsFromNode(::c4::yml::ConstNodeRef n);

}

#define ReadIntFromNode(node, id, min, max) \
  static_cast<int>(ReadLongFromNode((node), id, static_cast<long>(min), static_cast<long>(max)))
#define ReadIntFromNodeOrDefault(node, id, min, max, def) \
  static_cast<int>(ReadLongFromNodeOrDefault((node), id, static_cast<long>(min), static_cast<long>(max), static_cast<long>(def)))
#define WriteIntToNode(node, val) WriteLongToNode(node, static_cast<long>(val))

extern "C" {

  HRESULT WriteActionTypeToNode(::c4::yml::NodeRef n, ::roxyg::settings::ActionType value);
  HRESULT WriteTriggerFlagsToNode(::c4::yml::NodeRef node, ::roxyg::settings::TriggerFlags value);

}
