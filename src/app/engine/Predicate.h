#pragma once

namespace roxyg::window {
struct State;
}

namespace roxyg::engine {

struct Predicate {
  using DataType = std::variant<std::monostate, std::wstring>;
  using FunctionType = bool (*)(DataType const&, window::State&);

  FunctionType evaluate;
  DataType data;
};

using PredicateSet = boost::container::static_vector<Predicate const, 2>;

}
