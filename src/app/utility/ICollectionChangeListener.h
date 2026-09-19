#pragma once

namespace roxyg::utility {

enum class CollectionChangeType: uint_fast8_t {
  kReset = 0,
  kAdded = 1 << 0,
  kRemoved = 1 << 1,
  kUpdated = kAdded | kRemoved,
};

template<typename T>
struct CollectionChange {
  CollectionChangeType type;
  uint32_t index;
  T const* item;
};

template<typename T>
class ICollectionChangeListener {
public:
  virtual void onCollectionChanged(CollectionChange<T> const& change) noexcept = 0;
};

}

namespace magic_enum::customize {

template<>
struct enum_range<roxyg::utility::CollectionChangeType> {
  static constexpr bool is_flags = true;
};

}  // namespace magic_enum::customize
