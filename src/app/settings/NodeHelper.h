#pragma once
#include "String.h"

namespace roxyg::settings {

extern long ReadLongFromNode(c4::yml::ConstNodeRef node);
extern long ReadBoundedLongFromNode(c4::yml::ConstNodeRef node, long min_val, long max_val);
extern long ReadBoundedLongFromNodeOrDefault(c4::yml::ConstNodeRef node, long min_val, long max_val, long def_val);

extern float ReadFloatFromNode(c4::yml::ConstNodeRef node);
extern float ReadBoundedFloatFromNode(c4::yml::ConstNodeRef node, float min_val, float max_val);
extern float ReadBoundedFloatFromNodeOrDefault(c4::yml::ConstNodeRef node, float min_val, float max_val, float def_val);

extern std::string ReadStringFromNode(c4::yml::ConstNodeRef node);
extern std::wstring ReadStringAsUtf16FromNode(c4::yml::ConstNodeRef node);
extern StringAndCompareType ReadStringAndCompareTypeFromNode(c4::yml::ConstNodeRef node);

ROXYG_ALWAYS_INLINE int ReadIntFromNode(c4::yml::ConstNodeRef node) {
  return static_cast<int>(ReadLongFromNode(node));
}
ROXYG_ALWAYS_INLINE int ReadBoundedIntFromNode(c4::yml::ConstNodeRef node, int min_val, int max_val) {
  return static_cast<int>(ReadBoundedLongFromNode(
    node,
    static_cast<long>(min_val),
    static_cast<long>(max_val)));
}
ROXYG_ALWAYS_INLINE int ReadBoundedIntFromNodeOrDefault(c4::yml::ConstNodeRef node, int min_val, int max_val, int def_val) {
  return static_cast<int>(ReadBoundedLongFromNodeOrDefault(
    node,
    static_cast<long>(min_val),
    static_cast<long>(max_val),
    static_cast<long>(def_val)));
}

}
