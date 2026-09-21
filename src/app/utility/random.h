#pragma once
#include "macro.h"

#if defined(__GNUC__) || defined(__clang__)
# if defined(__i386__) || defined(__x86_64__)
#  include <x86intrin.h>
# endif
#elifdef _MSC_VER
# include <intrin.h>
#else
# include <chrono>
#endif

namespace roxyg::utility {

/// <summary>
/// Generates a deterministic 32-bit pseudo-random sequence using xorshift.
/// </summary>
class xorshift32 final {
public:
  using result_type = uint32_t;

  constexpr xorshift32(uint32_t seed = 2463534242u) noexcept
    : state_(seed == 0 ? 2463534242u : seed) {
  }

  constexpr uint32_t operator()() noexcept {
    uint32_t x = state_;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state_ = x;
  }

  static constexpr result_type min() noexcept { return std::numeric_limits<result_type>::min(); }
  static constexpr result_type max() noexcept { return std::numeric_limits<result_type>::max(); }

private:
  result_type state_;
};

/// <summary>
/// Generates a deterministic 64-bit pseudo-random sequence using xorshift.
/// </summary>
class xorshift64 final {
public:
  using result_type = uint64_t;

  constexpr xorshift64(uint64_t seed = 88172645463325252ull) noexcept
    : state_(seed == 0 ? 88172645463325252ull : seed) {
  }

  constexpr uint64_t operator()() noexcept {
    uint64_t x = state_;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state_ = x;
  }

  static constexpr result_type min() noexcept { return std::numeric_limits<result_type>::min(); }
  static constexpr result_type max() noexcept { return std::numeric_limits<result_type>::max(); }

private:
  result_type state_;
};

/// <summary>
/// Creates a mixed 32-bit seed from a timestamp counter and thread ID.
/// </summary>
/// <remarks>
/// Uses a MurmurHash3 fmix32-style finalizer after combining the inputs.
/// </remarks>
/// <param name="tsc64">The timestamp-counter value used as entropy.</param>
/// <param name="thread_id">The thread ID used as additional entropy.</param>
/// <returns>A deterministic mixed 32-bit seed.</returns>
ROXYG_ALWAYS_INLINE constexpr uint32_t make_seed32(uint64_t tsc64, uint32_t thread_id) {
  uint32_t tsc32 = static_cast<uint32_t>(tsc64);
  uint32_t seed = tsc32 ^ (thread_id * 0x9e3779b9u /* golden ratio constant: (1+sqrt(5))/2 */);
  seed ^= seed >> 16;
  seed *= 0x85ebca6bU;
  seed ^= seed >> 13;
  seed *= 0xc2b2ae35U;
  seed ^= seed >> 16;
  return seed;
}

/// <summary>
/// Creates a mixed 64-bit seed from a timestamp counter and thread ID.
/// </summary>
/// <remarks>
/// Uses the SplitMix64 Stafford variant 13 finalizer after combining the inputs.
/// </remarks>
/// <param name="tsc64">The timestamp-counter value used as entropy.</param>
/// <param name="thread_id">The thread ID used as additional entropy.</param>
/// <returns>A deterministic mixed 64-bit seed.</returns>
ROXYG_ALWAYS_INLINE constexpr uint64_t make_seed64(uint64_t tsc64, uint32_t thread_id) {
  uint64_t seed = tsc64 ^ (thread_id * 0x9e3779b97f4a7c15ull /* golden ratio constant: (1+sqrt(5))/2 */);
  seed ^= seed >> 30;
  seed *= 0xbf58476d1ce4e5b9ULL;
  seed ^= seed >> 27;
  seed *= 0x94d049bb133111ebULL;
  seed ^= seed >> 31;
  return seed;
}

/// <summary>
/// Creates a seed using the mixer preferred for the target architecture.
/// </summary>
/// <param name="tsc64">The timestamp-counter value used as entropy.</param>
/// <param name="thread_id">The thread ID used as additional entropy.</param>
/// <returns>A deterministic architecture-sized seed.</returns>
ROXYG_ALWAYS_INLINE constexpr size_t make_preferred_seed(uint64_t tsc64, uint32_t thread_id) {
#ifdef ROXYG_ARCH_32BIT
  return make_seed32(tsc64, thread_id);
#else
  return make_seed64(tsc64, thread_id);
#endif
}

/// <summary>
/// Reads a platform-specific high-resolution counter value.
/// </summary>
/// <remarks>
/// The returned value is suitable as entropy for seed generation. It is not a
/// portable wall-clock timestamp or a guaranteed unit of elapsed time.
/// </remarks>
/// <returns>The current platform counter value.</returns>
ROXYG_ALWAYS_INLINE uint64_t get_tsc() noexcept {
#if defined(__GNUC__) || defined(__clang__)
# if defined(__arm__) || defined(__aarch64__)
  uint64_t val;
  __asm__ __volatile__("mrs %0, cntvct_el0" : "=r" (val));
  return val;
# elif defined(__i386__) || defined(__x86_64__)
  return __rdtsc();
# else
  return static_cast<uint64_t>(::std::chrono::high_resolution_clock::now().time_since_epoch().count());
# endif
#elifdef _MSC_VER
# if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)
  return __ReadStatusReg(ARM64_CNTVCT);
# elif defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
  return __rdtsc();
# else
  return static_cast<uint64_t>(::std::chrono::high_resolution_clock::now().time_since_epoch().count());
# endif
#else
  return static_cast<uint64_t>(::std::chrono::high_resolution_clock::now().time_since_epoch().count());
#endif
}

}
