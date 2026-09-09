#include "pch.h"
#include "Filter.h"

#include "constants.h"
#include "NodeHelper.h"

using namespace roxyg::settings;

Filter::Filter(c4::yml::NodeRef node)
  : node_(std::move(node))
  , process_name_(ReadStringAndCompareTypeFromNode(node_[key::kProcessKey]))
  , window_class_(ReadStringAndCompareTypeFromNode(node_[key::kWindowClassKey]))
  , window_title_(ReadStringAndCompareTypeFromNode(node_[key::kWindowTitleKey])) {
}
