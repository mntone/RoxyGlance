#pragma once

namespace roxyg::utility {

/// <summary>
/// Caches values by key.
/// </summary>
/// <remarks>
/// Values are stored by value. Pointers and references returned by this cache
/// may be invalidated when the cache is modified, depending on the container
/// type. They must not be retained across cache modifications or used
/// concurrently with them.
///
/// Callers must ensure that all accesses to cached values have completed before
/// clearing, rebuilding, or destroying the cache.
/// </remarks>
template<typename Key, typename Value, typename Container = boost::unordered_flat_map<Key, Value>>
class ValueCache {
  ValueCache(ValueCache const&) = delete;
  ValueCache& operator=(ValueCache const&) = delete;

public:
  using KeyType = Key;
  using ValueType = Value;
  using ContainerType = Container;
  using IteratorType = typename Container::iterator;
  using ConstIteratorType = typename Container::const_iterator;

  /// <summary>
  /// Initializes an empty value cache.
  /// </summary>
  constexpr ValueCache() noexcept {
  }

  /// <summary>
  /// Gets the cached value for the specified item.
  /// </summary>
  /// <param name="key">The key of the value.</param>
  /// <returns>
  /// A pointer to the cached value, or <c>nullptr</c> if no value exists.
  /// The returned pointer must not be retained.
  /// </returns>
  [[nodiscard]] inline Value* get(Key key) noexcept {
    IteratorType it = storage_.find(key);
    return it != storage_.end() ? &it->second : nullptr;
  }

  /// <summary>
  /// Gets the cached value for the specified item.
  /// </summary>
  /// <param name="key">The key of the value.</param>
  /// <returns>
  /// A pointer to the cached value, or <c>nullptr</c> if no value exists.
  /// The returned pointer must not be retained.
  /// </returns>
  [[nodiscard]] inline Value const* get(Key key) const noexcept {
    ConstIteratorType it = storage_.find(key);
    return it != storage_.end() ? &it->second : nullptr;
  }

  /// <summary>
  /// Gets the cached value for the specified item, creating it if necessary.
  /// </summary>
  /// <param name="key">The key of the value.</param>
  /// <returns>
  /// A pointer to the existing or newly created value, or <c>nullptr</c> if
  /// memory allocation failed.
  /// </returns>
  /// <remarks>
  /// The returned pointer must not be retained across cache modifications or
  /// used concurrently with them.
  /// </remarks>
  [[nodiscard]] inline Value* getOrCreate(Key key) noexcept {
    try {
      auto [it, _] = storage_.try_emplace(key, key);
      return &it->second;
    } catch (std::bad_alloc const&) {
      return nullptr;
    }
  }

  /// <summary>
  /// Removes the cached value for the specified item.
  /// </summary>
  /// <param name="key">The key of the value.</param>
  /// <remarks>
  /// This operation may invalidate pointers and references to cached values.
  /// </remarks>
  inline void remove(Key key) noexcept {
    storage_.erase(key);
  }

protected:
  ContainerType storage_;
};

}
