#pragma once

#include "common.hpp"
#include "details/optional.hpp"
#include "details/string_view.hpp"
#include "details/mphf.hpp"
#include "entries.hpp"
#include "generators.hpp"
#include <bit>
#include <type_traits>
#include <utility>

namespace enchantum {

namespace details {
  constexpr std::pair<std::size_t, std::size_t> minmax_string_size(const string_view* begin, const string_view* const end)
  {
    using T     = std::size_t;
    auto minmax = std::pair<T, T>(std::numeric_limits<T>::max(), 0);

    for (; begin != end; ++begin) {
      const auto size = begin->size();
      minmax.first    = minmax.first < size ? minmax.first : size;
      minmax.second   = minmax.second > size ? minmax.second : size;
    }
    return minmax;
  }

  // Import hash function from mphf.hpp for use in cast optimization
  using enchantum::details::hash_string;

} // namespace details




template<Enum E>
[[nodiscard]] constexpr bool contains(const std::underlying_type_t<E> value) noexcept
{
  using T = std::underlying_type_t<E>;

  if (value < T(min<E>) || value > T(max<E>))
    return false;

  if constexpr (is_contiguous_bitflag<E>) {
    if constexpr (has_zero_flag<E>)
      if (value == 0)
        return true;

    return std::popcount(static_cast<std::make_unsigned_t<T>>(value)) == 1;
  }
  else if constexpr (is_contiguous<E>) {
    return true;
  }
  else {
    for (const auto v : values_generator<E>)
      if (static_cast<T>(v) == value)
        return true;
    return false;
  }
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const E value) noexcept
{
  return enchantum::contains<E>(static_cast<std::underlying_type_t<E>>(value));
}

template<Enum E>
[[nodiscard]] constexpr bool contains(const string_view name) noexcept
{
  constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
  if (const auto size = name.size(); size < minmax.first || size > minmax.second)
    return false;

  for (const auto s : names_generator<E>)
    if (s == name)
      return true;
  return false;
}


template<Enum E, std::predicate<string_view, string_view> BinaryPredicate>
[[nodiscard]] constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
{
  for (const auto s : names_generator<E>)
    if (binary_predicate(name, s))
      return true;
  return false;
}


namespace details {
  template<typename E>
  struct index_to_enum_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::size_t index) const noexcept
    {
      optional<E> ret;
      if (index < count<E>)
        ret.emplace(values_generator<E>[index]);
      return ret;
    }
  };

  struct enum_to_index_functor {
    template<Enum E>
    [[nodiscard]] constexpr optional<std::size_t> operator()(const E e) const noexcept
    {
      using T = std::underlying_type_t<E>;

      if constexpr (is_contiguous<E>) {
        if (enchantum::contains(e)) {
          return optional<std::size_t>(std::size_t(T(e) - T(min<E>)));
        }
      }
      else if constexpr (is_contiguous_bitflag<E>) {
        if (enchantum::contains(e)) {
          constexpr bool has_zero = has_zero_flag<E>;
          if constexpr (has_zero)
            if (static_cast<T>(e) == 0)
              return optional<std::size_t>(0); // assumes 0 is the index of value `0`

          using U = std::make_unsigned_t<T>;
          return has_zero + std::countr_zero(static_cast<U>(e)) - std::countr_zero(static_cast<U>(values_generator<E>[has_zero]));
        }
      }
      else {
        for (std::size_t i = 0; i < count<E>; ++i) {
          if (values_generator<E>[i] == e)
            return i;
        }
      }
      return optional<std::size_t>();
    }
  };


  template<Enum E>
  struct cast_functor {
    [[nodiscard]] constexpr optional<E> operator()(const std::underlying_type_t<E> value) const noexcept
    {
      optional<E> a; // rvo not that it really matters
      if (!enchantum::contains<E>(value))
        return a;
      a.emplace(static_cast<E>(value));
      return a;
    }

    [[nodiscard]] constexpr optional<E> operator()(const string_view name) const noexcept
    {
      optional<E> a; // rvo not that it really matters

      constexpr auto minmax = details::minmax_string_size(names<E>.data(), names<E>.data() + names<E>.size());
      if (const auto size = name.size(); size < minmax.first || size > minmax.second)
        return a; // nullopt

      // Use optimized lookup for small to medium enums where it's likely beneficial
      if constexpr (count<E> <= 100) {
        const auto lookup_result = optimized_string_lookup<E>(name);
        if (lookup_result < count<E>) {
          a.emplace(values_generator<E>[lookup_result]);
        }
        return a;
      } else {
        // Fallback to linear search for larger enums
        for (std::size_t i = 0; i < count<E>; ++i) {
          if (names_generator<E>[i] == name) {
            a.emplace(values_generator<E>[i]);
            return a;
          }
        }
        return a; // nullopt
      }
    }

    template<std::predicate<string_view, string_view> BinaryPred>
    [[nodiscard]] constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
    {
      optional<E> a; // rvo not that it really matters
      for (std::size_t i = 0; i < count<E>; ++i) {
        if (binary_predicate(name, names_generator<E>[i])) {
          a.emplace(values_generator<E>[i]);
          return a;
        }
      }
      return a;
    }

  private:
    template<Enum EnumType>
    [[nodiscard]] static constexpr std::size_t optimized_string_lookup(const string_view name) noexcept
    {
      constexpr auto names_array = names<EnumType>;
      constexpr std::size_t N = names_array.size();
      
      if constexpr (N == 0) {
        return N;
      }
      
      // For very small enums, linear search is already optimal
      if constexpr (N <= 5) {
        for (std::size_t i = 0; i < N; ++i) {
          if (names_array[i] == name) {
            return i;
          }
        }
        return N;
      } else {
        // For larger enums, use hash table with perfect hash if possible
        constexpr auto hash_table_result = generate_optimal_hash_table<N>(names_array);
        
        if constexpr (hash_table_result.is_perfect) {
          // Perfect hash table - single lookup, no collisions
          const auto hash_val = hash_string(name, hash_table_result.seed) % N;
          const auto string_index = hash_table_result.table[hash_val];
          
          if (string_index < N && names_array[string_index] == name) {
            return string_index;
          }
          return N; // Not found
        } else {
          // Fallback hash table with linear probing  
          const auto hash_val = hash_string(name, hash_table_result.seed) % N;
          
          // Linear probing for collision resolution
          for (std::size_t probe = 0; probe < N; ++probe) {
            const auto pos = (hash_val + probe) % N;
            const auto string_index = hash_table_result.table[pos];
            
            if (string_index == N) {
              return N; // Not found (empty slot)
            }
            
            if (names_array[string_index] == name) {
              return string_index;
            }
          }
          
          return N; // Not found
        }
      }
    }
    
    template<std::size_t N>
    struct hash_table_result {
      std::array<std::size_t, N> table{};
      std::uint32_t seed = 0;
      bool is_perfect = false;
      
      constexpr hash_table_result() {
        for (std::size_t i = 0; i < N; ++i) {
          table[i] = N; // N means "empty"
        }
      }
    };
    
    template<std::size_t N>
    static constexpr hash_table_result<N> generate_optimal_hash_table(const std::array<string_view, N>& strings) noexcept
    {
      hash_table_result<N> result;
      
      // Try to find a perfect hash function (no collisions)
      for (std::uint32_t seed = 0; seed < 200; ++seed) {
        std::array<std::size_t, N> temp_table{};
        std::array<bool, N> used{};
        bool is_perfect = true;
        
        // Initialize temp table
        for (std::size_t i = 0; i < N; ++i) {
          temp_table[i] = N; // N means "empty"
        }
        
        // Try to place all strings with this seed
        for (std::size_t i = 0; i < N && is_perfect; ++i) {
          const auto hash_val = hash_string(strings[i], seed) % N;
          if (used[hash_val]) {
            is_perfect = false; // Collision detected
          } else {
            temp_table[hash_val] = i;
            used[hash_val] = true;
          }
        }
        
        if (is_perfect) {
          result.table = temp_table;
          result.seed = seed;
          result.is_perfect = true;
          return result;
        }
      }
      
      // If no perfect hash found, create hash table with linear probing
      result.seed = 17; // Fixed seed for deterministic behavior
      
      // Initialize table
      for (std::size_t i = 0; i < N; ++i) {
        result.table[i] = N; // N means "empty"
      }
      
      // Place strings using linear probing
      for (std::size_t i = 0; i < N; ++i) {
        const auto hash_val = hash_string(strings[i], result.seed) % N;
        
        // Find next available slot using linear probing
        for (std::size_t probe = 0; probe < N; ++probe) {
          const auto pos = (hash_val + probe) % N;
          if (result.table[pos] == N) {
            result.table[pos] = i;
            break;
          }
        }
      }
      
      result.is_perfect = false;
      return result;
    }
  };

} // namespace details

template<Enum E>
inline constexpr details::index_to_enum_functor<E> index_to_enum{};

inline constexpr details::enum_to_index_functor enum_to_index{};

template<Enum E>
inline constexpr details::cast_functor<E> cast{};


namespace details {
  struct to_string_functor {
    template<Enum E>
    [[nodiscard]] constexpr string_view operator()(const E value) const noexcept
    {
      if (const auto i = enchantum::enum_to_index(value))
        return names_generator<E>[*i];
      return string_view();
    }
  };

} // namespace details
inline constexpr details::to_string_functor to_string{};


} // namespace enchantum