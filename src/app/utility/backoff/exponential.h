#pragma once
#include "jitter.h"

namespace roxyg::utility {

namespace detail {

template<std::integral Int, Int MaxVal, float Scale>
  requires (
    Scale > 1.f
    && 0 <= MaxVal
    && static_cast<double>(MaxVal) * static_cast<double>(Scale) <= static_cast<double>(std::numeric_limits<Int>::max())
  )
struct scale_and_clamp {
  static constexpr float scale{Scale};
  constexpr Int operator()(Int value) const noexcept {
    return std::min(
      static_cast<Int>(static_cast<float>(value) * Scale),
      MaxVal
    );
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 9)
struct scale_and_clamp<Int, MaxVal, 1.125f> {
  static constexpr float scale{1.125f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 9) >> 3, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 5)
struct scale_and_clamp<Int, MaxVal, 1.25f> {
  static constexpr float scale{1.25f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 5) >> 2, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 11)
struct scale_and_clamp<Int, MaxVal, 1.375f> {
  static constexpr float scale{1.375f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 11) >> 3, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 3)
struct scale_and_clamp<Int, MaxVal, 1.5f> {
  static constexpr float scale{1.5f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 3) >> 1, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 13)
struct scale_and_clamp<Int, MaxVal, 1.625f> {
  static constexpr float scale{1.625f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 13) >> 3, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 7)
struct scale_and_clamp<Int, MaxVal, 1.75f> {
  static constexpr float scale{1.75f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 7) >> 2, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 15)
struct scale_and_clamp<Int, MaxVal, 1.875f> {
  static constexpr float scale{1.875f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 15) >> 3, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 2)
struct scale_and_clamp<Int, MaxVal, 2.f> {
  static constexpr float scale{2.f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min(value << 1, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 5)
struct scale_and_clamp<Int, MaxVal, 2.5f> {
  static constexpr float scale{2.5f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 5) >> 1, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 3)
struct scale_and_clamp<Int, MaxVal, 3.f> {
  static constexpr float scale{3.f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min(value * 3, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 7)
struct scale_and_clamp<Int, MaxVal, 3.5f> {
  static constexpr float scale{3.5f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min((value * 7) >> 1, MaxVal);
  }
};
template<std::integral Int, Int MaxVal>
  requires (0 <= MaxVal && MaxVal <= std::numeric_limits<Int>::max() / 4)
struct scale_and_clamp<Int, MaxVal, 4.f> {
  static constexpr float scale{4.f};
  constexpr Int operator()(Int value) const noexcept {
    return std::min(value << 2, MaxVal);
  }
};

template<typename Int, Int MaxVal, float Scale>
concept scale_and_clampable = requires {
  typename scale_and_clamp<Int, MaxVal, Scale>;
};

}  // namespace detail

template<std::integral Int, Int MinDelay, Int MaxDelay, float Factor, typename JitterGenerator = no_jitter_generator>
  requires (
    Factor > 1.f
    && 0 <= MinDelay
    && MinDelay <= MaxDelay
    && detail::scale_and_clampable<Int, MaxDelay, Factor>
  )
class exponential_backoff final: public JitterGenerator {
public:
  static constexpr Int min_delay{MinDelay};
  static constexpr Int max_delay{MaxDelay};
  static constexpr float factor{Factor};

  constexpr exponential_backoff(JitterGenerator jitter = {}) noexcept
    : JitterGenerator(std::move(jitter))
    , current_(MinDelay) {
  }

  ROXYG_ALWAYS_INLINE constexpr void reset() noexcept {
    current_ = MinDelay;
  }

  [[nodiscard]] ROXYG_ALWAYS_INLINE constexpr Int current() const noexcept {
    return current_;
  }

  [[nodiscard]] ROXYG_ALWAYS_INLINE constexpr Int next() noexcept {
    using scalar = detail::scale_and_clamp<Int, MaxDelay, Factor>;

    Int const next = scalar{}(current_);
    Int const clamp = std::min(next, MaxDelay);
    current_ = clamp;

    if constexpr (JitterGenerator::enabled) {
      return std::min(this->random(clamp), MaxDelay);
    } else {
      return clamp;
    }
  }

private:
  Int current_;
};

template<uint32_t MinDelay, uint32_t MaxDelay, float Factor, typename JitterGenerator = no_jitter_generator>
using uint32_exponential_backoff = exponential_backoff<uint32_t, MinDelay, MaxDelay, Factor, JitterGenerator>;

}
