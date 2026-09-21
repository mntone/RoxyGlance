#pragma once
#include "../macro.h"
#include "../random.h"

namespace roxyg::utility {

template<typename T, float Ratio>
  requires (0.f <= Ratio && Ratio <= 1.f)
class jitter_generator {
public:
  static constexpr bool enabled{true};

  ROXYG_ALWAYS_INLINE jitter_generator(T rng = {})
    : rng_(std::move(rng)) {
  };

  template<typename U>
  constexpr U random(U max) noexcept {
    U base, smax;
    if constexpr (Ratio == 0.25f) {
      smax = max >> 2;
      base = max - smax;
    } else if constexpr (Ratio == 0.5f) {
      smax = max >> 1;
      base = max - smax;
    } else if constexpr (Ratio == 0.75f) {
      base = max >> 2;
      smax = max - base;
    } else if constexpr (Ratio == 1.f) {
      base = 0;
      smax = max;
    } else {
      smax = static_cast<U>(Ratio * max);
      base = max - smax;
    }

    std::uniform_int_distribution<U> dist(0, smax);
    return base + dist(rng_);
  }

private:
  T rng_;
};

template<>
class jitter_generator<void, 0.f> {
public:
  static constexpr bool enabled{false};

  constexpr jitter_generator() noexcept {
  }

  template<typename U>
  constexpr U random(U max = std::numeric_limits<U>::max()) noexcept {
    return max;
  }
};

using no_jitter_generator = jitter_generator<void, 0.f>;

template<float Ratio> using xorshift32_jitter_generator = jitter_generator<xorshift32, Ratio>;
using xorshift32_equal_jitter_generator = xorshift32_jitter_generator<0.5f>;
using xorshift32_full_jitter_generator = xorshift32_jitter_generator<1.f>;

template<float Ratio> using xorshift64_jitter_generator = jitter_generator<xorshift64, Ratio>;
using xorshift64_equal_jitter_generator = xorshift64_jitter_generator<0.5f>;
using xorshift64_full_jitter_generator = xorshift64_jitter_generator<1.f>;

#ifdef ROXYG_ARCH_32BIT
template<float Ratio> using preferred_xorshift_jitter_generator = jitter_generator<xorshift32, Ratio>;
#else
template<float Ratio> using preferred_xorshift_jitter_generator = jitter_generator<xorshift64, Ratio>;
#endif
using preferred_xorshift_equal_jitter_generator = preferred_xorshift_jitter_generator<0.5f>;
using preferred_xorshift_full_jitter_generator = preferred_xorshift_jitter_generator<1.f>;

}
