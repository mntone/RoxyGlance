#pragma once
#include "String.h"

namespace roxyg::settings {

extern long ReadLongFromNode(c4::yml::ConstNodeRef node);
extern long ReadBoundedLongFromNode(c4::yml::ConstNodeRef node, long min_val, long max_val);

extern float ReadFloatFromNode(c4::yml::ConstNodeRef node);
extern float ReadBoundedFloatFromNode(c4::yml::ConstNodeRef node, float min_val, float max_val);
extern float ReadBoundedFloatFromNodeOrDefault(c4::yml::ConstNodeRef node, float min_val, float max_val, float def_val);

extern std::string ReadStringFromNode(c4::yml::ConstNodeRef node);
extern std::wstring ReadStringAsUtf16FromNode(c4::yml::ConstNodeRef node);
extern StringAndCompareType ReadStringAndCompareTypeFromNode(c4::yml::ConstNodeRef node);

}
