#pragma once
#include "String.h"

namespace roxyg::settings {

long ReadLongFromNode(c4::yml::ConstNodeRef node, long min_val, long max_val);
long ReadLongFromNodeOrDefault(c4::yml::ConstNodeRef node, long min_val, long max_val, long def_val);

float ReadFloatFromNode(c4::yml::ConstNodeRef node, float min_val, float max_val);
float ReadFloatFromNodeOrDefault(c4::yml::ConstNodeRef node, float min_val, float max_val, float def_val);

std::string ReadStringFromNode(c4::yml::ConstNodeRef node);
std::wstring ReadStringAsUtf16FromNode(c4::yml::ConstNodeRef node);
StringAndCompareType ReadStringAndCompareTypeFromNode(c4::yml::ConstNodeRef node);

void WriteLongToNode(c4::yml::NodeRef node, long val);
void WriteFloatToNode(c4::yml::NodeRef node, float val);
void WriteStringAndCompareTypeToNode(c4::yml::NodeRef node, c4::csubstr key, StringAndCompareType val);

}

#define ReadIntFromNode(node, min, max) \
  static_cast<int>(ReadLongFromNode((node), static_cast<long>(min), static_cast<long>(max)))
#define ReadIntFromNodeOrDefault(node, min, max, def) \
  static_cast<int>(ReadLongFromNodeOrDefault((node), static_cast<long>(min), static_cast<long>(max), static_cast<long>(def)))
#define WriteIntToNode(node, val) WriteLongToNode(node, static_cast<long>(val))

#include "Action.h"
#include "TriggerFlags.h"

extern "C" {

  HRESULT ReadActionTypeFromNode(::c4::yml::ConstNodeRef n, ::roxyg::settings::ActionType* value);
  HRESULT ReadTriggerFlagsFromNode(::c4::yml::ConstNodeRef n, ::roxyg::settings::TriggerFlags* value);
  HRESULT WriteActionTypeToNode(::c4::yml::NodeRef n, ::roxyg::settings::ActionType value);
  HRESULT WriteTriggerFlagsToNode(::c4::yml::NodeRef node, ::roxyg::settings::TriggerFlags value);

}
