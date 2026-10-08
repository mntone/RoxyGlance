#pragma once

#include "../context/OperationContext.h"

namespace roxyg::window {
struct State;
}

namespace roxyg::engine {

class IOperation {
public:
  virtual ~IOperation() noexcept = default;
  [[nodiscard]] virtual winrt::hresult execute(OperationContext& ctx, window::State& windowState) noexcept = 0;
};

using Operation = std::shared_ptr<IOperation>;

}
