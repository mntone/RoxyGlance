#pragma once
#include "ParseError.h"

namespace roxyg::settings::detail {

std::wstring makeRuleDisplayName(c4::yml::ConstNodeRef rule, ParseError err, size_t index);
winrt::hstring makeParseErrorMessage(ParseError err, std::wstring_view rule_name);

}
