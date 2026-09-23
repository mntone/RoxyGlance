#pragma once

namespace roxyg::utility {

template<std::integral Int, Int Delay>
  requires (Delay >= 0)
class fixed_backoff final {
public:
  static constexpr Int delay{Delay};

  ROXYG_ALWAYS_INLINE constexpr void reset() noexcept {}

  [[nodiscard]] ROXYG_ALWAYS_INLINE constexpr Int current() const noexcept {
    return Delay;
  }

  [[nodiscard]] ROXYG_ALWAYS_INLINE Int next() noexcept {
    return Delay;
  }
};

template<std::integral Int> using immediate_backoff = fixed_backoff<Int, 0>;
using uint32_immediate_backoff = immediate_backoff<uint32_t>;

template<uint32_t Delay> using uint32_fixed_backoff = fixed_backoff<uint32_t, Delay>;

}
