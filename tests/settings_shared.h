#pragma once

#include <c4/std/std.hpp>
#include <c4/yml/yml.hpp>

namespace test::roxyg::settings {

static c4::yml::NodeRef load_yaml(c4::yml::Tree& tree, std::string_view yaml) {
  c4::yml::parse_in_arena(c4::to_csubstr(yaml), &tree);
  c4::yml::NodeRef root = tree.rootref();
  if (root.invalid()) {
    throw std::bad_exception();
  }
  return root;
}

template<typename T>
T load_yaml_as(c4::yml::Tree& tree, std::string_view yaml) {
  c4::yml::NodeRef root = load_yaml(tree, yaml);
  return T(root);
}

template<typename Func, typename... Args>
  requires std::invocable<Func, c4::yml::ConstNodeRef, Args...>
auto loadFromYaml(std::string_view yaml, Func&& fn, Args&&... args)
-> std::invoke_result_t<Func, c4::yml::ConstNodeRef, Args&&...> {
  c4::yml::Tree tree;
  c4::yml::parse_in_arena(c4::to_csubstr(yaml), &tree);
  c4::yml::ConstNodeRef root = tree.rootref();
  if (root.invalid()) {
    throw std::bad_exception();
  }
  return std::invoke(std::forward<Func>(fn), root, std::forward<Args>(args)...);
}

template<size_t Size = 4>
using TestVec = boost::container::small_vector<std::string_view, Size>;

template<size_t Size = 4>
static TestVec<Size> keys_to_vec(c4::yml::ConstNodeRef n) {
  TestVec<Size> actual;
  for (c4::yml::ConstNodeRef c : n.children()) {
    c4::csubstr const value{c.key()};
    actual.emplace_back(value.str, value.len);
  }
  return actual;
}

template<size_t Size = 4>
static TestVec<Size> vals_to_vec(c4::yml::ConstNodeRef n) {
  TestVec<Size> actual;
  for (c4::yml::ConstNodeRef c : n.children()) {
    c4::csubstr const value{c.val()};
    actual.emplace_back(value.str, value.len);
  }
  return actual;
}

}

#define TEST_YAML(node_name, yaml) \
  ::c4::yml::Tree __tree; \
  ::c4::yml::NodeRef node_name{::test::roxyg::settings::load_yaml(__tree, yaml)};
