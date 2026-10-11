#include "pch.h"
#include "Filter.h"

#include "constants.h"
#include "NodeHelper.h"

using namespace c4::yml;
using namespace roxyg::settings;

namespace {

void writeFilterStringMatchType(NodeRef node, c4::csubstr key, StringAndMatchType const& val) {
  if (val.second.empty() && val.first == StringMatchType::kContains) {
    if (node.has_child(key)) {
      NodeRef child{node[key]};
      node.remove_child(child);
    }
  } else {
    WriteStringAndMatchTypeToNode(node[key], val);
  }
}

}

Filter::Filter(NodeRef node)
  : node_(std::move(node))
  , process_image_name_(ReadStringAndMatchTypeFromNode(node_[key::kProcessKey], KeyId::kProcessImageName))
  , window_class_(ReadStringAndMatchTypeFromNode(node_[key::kWindowClassKey], KeyId::kWindowClass))
  , window_title_(ReadStringAndMatchTypeFromNode(node_[key::kWindowTitleKey], KeyId::kWindowTitle)) {
}

void Filter::setProcessImageName(StringAndMatchType val) {
  writeFilterStringMatchType(node_, key::kProcessKey, val);
  process_image_name_ = std::move(val);
}

void Filter::setWindowClass(StringAndMatchType val) {
  writeFilterStringMatchType(node_, key::kWindowClassKey, val);
  window_class_ = std::move(val);
}

void Filter::setWindowTitle(StringAndMatchType val) {
  writeFilterStringMatchType(node_, key::kWindowTitleKey, val);
  window_title_ = std::move(val);
}
