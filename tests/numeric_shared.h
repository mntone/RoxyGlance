#pragma once

namespace test::roxyg::numeric {

using namespace ::roxyg::numeric;

template <typename Type, std::size_t Dimension, std::size_t Align = std::bit_ceil(sizeof(Type)* Dimension)>
struct TypedVectorConfig final {
  using T = Type;
  static constexpr std::size_t N = Dimension;
  static constexpr std::size_t A = Align;
};

class TypedVectorNameGenerator final {
public:
  template<typename T>
  static std::string GetName(int) {
    if constexpr (std::is_same_v<T, TypedVectorConfig<int, 2>>) {
      return "int2";
    }
#if INT_MAX == 0x7fffffff
    if constexpr (std::is_same_v<T, TypedVectorConfig<int, 2, 16>>) {
      return "fast_int2";
    }
#endif
    if constexpr (std::is_same_v<T, TypedVectorConfig<int, 3>>) {
      return "int3";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<int, 4>>) {
      return "int4";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<long, 2>>) {
      return "long2";
    }
#if LONG_MAX == 0x7fffffff
    if constexpr (std::is_same_v<T, TypedVectorConfig<long, 2, 16>>) {
      return "long2_fast";
    }
#endif
    if constexpr (std::is_same_v<T, TypedVectorConfig<long, 3>>) {
      return "long3";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<long, 4>>) {
      return "long4";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<float, 2>>) {
      return "float2";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<float, 2, 16>>) {
      return "float2_fast";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<float, 3>>) {
      return "float3";
    }
    if constexpr (std::is_same_v<T, TypedVectorConfig<float, 4>>) {
      return "float4";
    }
  }
};

}  // namespace test::roxyg::numeric
