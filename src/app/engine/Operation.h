#pragma once

#include "../context/OperationContext.h"

namespace roxyg::window {
struct State;
}

namespace roxyg::engine {

class Operation {
public:
  virtual ~Operation() noexcept = 0;
  [[nodiscard]] virtual winrt::hresult execute(OperationContext& ctx, window::State& windowState) noexcept = 0;
};

using OperationSet = boost::container::small_vector<std::shared_ptr<Operation>, 1>;

}
