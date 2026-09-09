#pragma once

#include <c4/std/std.hpp>
#include <c4/yml/yml.hpp>

namespace test::roxyg::settings {

template<typename T>
T loadFromYaml(std::string_view yaml) {
  c4::yml::Tree tree;
  c4::yml::parse_in_arena(c4::to_csubstr(yaml), &tree);
  c4::yml::NodeRef root = tree.rootref();
  if (root.invalid()) {
    throw std::bad_exception();
  }
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

}
