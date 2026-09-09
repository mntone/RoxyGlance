#pragma once
#include "Operation.h"

#include "../numeric/numeric.h"

namespace roxyg::engine {

class AbsoluteMoveAndResizeOperation final: public Operation {
  AbsoluteMoveAndResizeOperation(AbsoluteMoveAndResizeOperation const&) = delete;
  AbsoluteMoveAndResizeOperation& operator=(AbsoluteMoveAndResizeOperation const&) = delete;

public:
  explicit constexpr AbsoluteMoveAndResizeOperation(numeric::long4 bounds) noexcept
    : bounds_(std::move(bounds)) {
  }
  virtual ~AbsoluteMoveAndResizeOperation() noexcept = default;

  winrt::hresult execute(OperationContext& ctx, window::State& windowState) noexcept override;

private:
  numeric::long4 const bounds_;
};

class RelativeMoveAndResizeOperation final: public Operation {
  RelativeMoveAndResizeOperation(RelativeMoveAndResizeOperation const&) = delete;
  RelativeMoveAndResizeOperation& operator=(RelativeMoveAndResizeOperation const&) = delete;

public:
  explicit constexpr RelativeMoveAndResizeOperation(numeric::float4 relative_bounds) noexcept
    : dirty_(true)
    , bounds_(numeric::long4::make(0, 0, 0, 0))
    , relative_bounds_(std::move(relative_bounds)) {
  }
  virtual ~RelativeMoveAndResizeOperation() noexcept = default;

  winrt::hresult execute(OperationContext& ctx, window::State& windowState) noexcept override;

private:
  bool dirty_;
  numeric::long4 bounds_;
  numeric::float4 relative_bounds_;
};

}
