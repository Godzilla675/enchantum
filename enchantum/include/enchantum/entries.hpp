#pragma once

#include "details/string_view.hpp"
#if defined(__RESHARPER__)
  #include "details/enchantum_resharper_cpp.hpp"
#elif defined(__NVCOMPILER)
  #include "details/enchantum_nvcc.hpp"
#elif defined(__clang__)
  #include "details/enchantum_clang.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
  #include "details/enchantum_gcc.hpp"
#elif defined(_MSC_VER)
  #include "details/enchantum_msvc.hpp"
#else
  #error unsupported compiler please open an issue for enchantum
#endif

#include "common.hpp"
#include <type_traits>
#include <utility>
#include <array>
#include <climits>
#include "details/constexpr_if.hpp"

// ENCHANTUM_NODISCARD is defined in common.hpp

#ifndef ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY
  #define ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY 2
#endif
#if ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY < 0
  #error ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY must not be a negative number.
#endif
namespace enchantum {

#ifdef __cpp_lib_to_underlying
using ::std::to_underlying;
#else
template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
ENCHANTUM_NODISCARD constexpr auto to_underlying(const E e) noexcept
  -> std::underlying_type_t<E>
{
  return static_cast<std::underlying_type_t<E>>(e);
}
#endif


namespace details {
  template<typename Int>
  constexpr std::size_t get_index_sequence_max(
    const bool        is_bitflag,
    const bool        has_fixed_underlying,
    const std::size_t sizeof_enum,
    const Int         min,
    const Int         max,
    const bool        is_signed)
  {
    (void)has_fixed_underlying;
    if (!is_bitflag)
      return static_cast<std::size_t>(max - min + 1);

#if defined(__clang__) && __clang_major__ >= 20
    if (!has_fixed_underlying) {
      auto        v = max;
      std::size_t r = 1;
      while (v >>= 1)
        r++;
      return r;
    }
#endif
    return (sizeof_enum * CHAR_BIT) - is_signed;
  }

  template<typename E, typename StringLengthType, std::size_t Size>
  struct FinalReflectionResult {
    std::array<E, Size> values{};
    // +1 for easier iteration on on last string
    std::array<StringLengthType, Size + 1> string_indices{};
  };

  template<typename E, bool NullTerminated, auto Min = enum_traits<E>::min, decltype(Min) Max = enum_traits<E>::max>
  static constexpr auto reflection_data_impl = details::reflect<E, NullTerminated, Min>(
    std::make_index_sequence<details::get_index_sequence_max(is_bitflag<E>,
                                                             has_fixed_underlying_type<E>,
                                                             sizeof(E),
                                                             Min,
                                                             Max,
                                                             std::is_signed_v<std::underlying_type_t<E>>)>{});


  // Thanks https://en.cppreference.com/w/cpp/utility/intcmp.html
  template<typename T, typename U>
  constexpr bool cmp_less(const T t, const U u) noexcept
  {
    return details::constexpr_if_else<std::is_signed<T>::value == std::is_signed<U>::value>(
        [&]() { return t < u; },
        [&]() {
            return details::constexpr_if_else<std::is_signed<T>::value>(
                [&]() { return t < 0 || std::make_unsigned_t<T>(t) < u; },
                [&]() { return u >= 0 && t < std::make_unsigned_t<U>(u); }
            );
        }
    );
  }

  template<typename U>
  constexpr bool cmp_less(const bool t, const U u) noexcept
  {
    return details::cmp_less(int(t), u);
  }

  template<typename T>
  constexpr bool cmp_less(const T t, const bool u) noexcept
  {
    return details::cmp_less(t, int(u));
  }

  constexpr bool cmp_less(const bool t, const bool u) noexcept { return int(t) < int(u); }

  template<typename T, typename U>
  constexpr T ClampToRange(U u)
  {
    using L = std::numeric_limits<T>;
    if (details::cmp_less((L::max)(), u))
      return (L::max)();
    if (details::cmp_less(u, (L::min)()))
      return (L::min)();
    return T(u);
  }
  template<typename E, bool NullTerminated>
  constexpr auto get_reflection_data() noexcept
  {
    constexpr auto elements = reflection_data_impl<E, NullTerminated>;
    using StringLengthType = std::conditional_t<(elements.total_string_length < UINT8_MAX), std::uint8_t, std::uint16_t>;

#if ENCHANTUM_CHECK_OUT_OF_BOUNDS_BY >= 2
    // TODO: Implement check for C++14
#endif

    FinalReflectionResult<E, StringLengthType, elements.valid_count> ret{}; // Added {} for C++14 aggregate init safety? C++14 has aggregate init.
    std::size_t                                                      i            = 0;
    StringLengthType                                                 string_index = 0;
    for (; i < elements.valid_count; ++i) {
      ret.values[i] = static_cast<E>(elements.values[i]);
      // "aabc"

      ret.string_indices[i] = string_index;
#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
      // false positives from T += T
      // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif
      string_index += static_cast<StringLengthType>(elements.string_lengths[i] + NullTerminated);
#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
    }
    ret.string_indices[i] = string_index;
    return ret;
  }


  template<typename E, bool NullTerminated>
  static constexpr auto reflection_data_string_storage = details::reflection_data_impl<E, NullTerminated>.strings;

  template<typename E, bool NullTerminated>
  static constexpr auto reflection_data = details::get_reflection_data<E, NullTerminated>();

  template<typename E, bool NullTerminated>
  static constexpr auto reflection_string_indices = reflection_data<E, NullTerminated>.string_indices;

  // Helper to construct Pair for get_entries
  template<typename E, typename Pair, bool NullTerminated>
  constexpr Pair make_entry(std::size_t i) {
      // Access reflected data
      // We cannot use 's' directly from reflection data because it is not string_view.
      // We need to construct StringView (from Pair).
      // We assume Pair is {E, StringView} aggregate.

      // Deduce StringView type from Pair if possible?
      // Or just assume we can construct Pair with {E, enchantum::string_view}.
      // But the test uses std::string_view.

      // Let's use reflection_data_string_storage and indices.
      const auto* strings = reflection_data_string_storage<E, NullTerminated>;
      const auto& indices = reflection_string_indices<E, NullTerminated>;

      // We need to return Pair{ value, string }.
      // string needs to be constructed.
      // We construct enchantum::string_view then cast? Or direct construction.

      return Pair{
          reflection_data<E, NullTerminated>.values[i],
          enchantum::string_view(strings + indices[i], indices[i+1] - indices[i] - NullTerminated)
      };
  }

  template<typename E, typename Pair, bool NullTerminated, std::size_t... Is>
  constexpr std::array<Pair, sizeof...(Is)> get_entries_impl(std::index_sequence<Is...>)
  {
      return {{ make_entry<E, Pair, NullTerminated>(Is)... }};
  }

  template<typename E, typename Pair, bool NullTerminated, typename Reflected = int>
  constexpr auto get_entries()
  {
    constexpr auto size = reflection_data<std::remove_cv_t<E>, NullTerminated>.values.size();
    static_assert(size != 0, "enchantum failed to reflect this enum.");

    return get_entries_impl<std::remove_cv_t<E>, Pair, NullTerminated>(std::make_index_sequence<size>{});
  }
} // namespace details

namespace details {
  template<typename E>
  constexpr auto get_values() noexcept
  {
#if defined(__NVCOMPILER)
    return details::reflection_data<E, true>.values;
#else
    return details::reflection_data<std::remove_cv_t<E>, true>.values;
#endif
  }

  template<typename E, typename String, bool NullTerminated>
  constexpr auto get_names() noexcept
  {
#if defined(__NVCOMPILER)
    const auto strings   = details::reflection_data_string_storage<E, NullTerminated>.data();
    const auto indices   = details::reflection_data<E, NullTerminated>.string_indices;
#else
    constexpr auto strings   = details::reflection_data_string_storage<std::remove_cv_t<E>, NullTerminated>;
    constexpr auto indices   = details::reflection_data<std::remove_cv_t<E>, NullTerminated>.string_indices;
#endif
    constexpr auto size      = indices.size() - 1;

    std::array<String, size> ret{};
    for (std::size_t i = 0; i < size; ++i) {
      ret[i] = String(strings + indices[i], indices[i + 1] - indices[i] - NullTerminated);
    }
    return ret;
  }

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
static constexpr auto values = details::get_values<E>();

#ifdef __cpp_concepts
template<Enum E, typename String = string_view, bool NullTerminated = true>
#else
template<typename E, typename String = string_view, bool NullTerminated = true, std::enable_if_t<is_enum_v<E>, int> = 0>
#endif
static constexpr auto names = details::get_names<E, String, NullTerminated>();


#ifdef __cpp_concepts
template<Enum E, typename Pair = std::pair<E, enchantum::string_view>, bool NullTerminated = true>
#else
template<typename E,
         typename Pair                            = std::pair<E, enchantum::string_view>,
         bool NullTerminated                      = true,
         std::enable_if_t<is_enum_v<E>, int> = 0>
#endif
static constexpr auto entries = enchantum::details::get_entries<E, Pair, NullTerminated>();

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
static constexpr std::size_t count = values<E>.size();

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
static constexpr auto min = std::get<0>(values<E>);

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
static constexpr auto max = std::get<count<E> - 1>(values<E>);


// has_zero_flag implementation logic
// In C++17 it used a lambda initialized variable with if constexpr.
// We can use a template struct helper or constexpr function.

namespace details {
  template <typename E>
  constexpr bool check_has_zero_flag(std::true_type) {
    for (const auto v : values<E>)
      if (static_cast<std::underlying_type_t<E>>(v) == 0)
        return true;
    return false;
  }

  template <typename E>
  constexpr bool check_has_zero_flag(std::false_type) {
    return false;
  }
}

template<typename E>
static constexpr bool has_zero_flag = details::check_has_zero_flag<E>(std::bool_constant<is_bitflag<E>>{});

template<typename E>
static constexpr bool is_contiguous = static_cast<std::size_t>(
                                        enchantum::to_underlying(max<E>) - enchantum::to_underlying(min<E>)) +
    1 ==
  count<E>;


// is_contiguous_bitflag implementation logic

namespace details {
  template <typename E>
  constexpr bool check_is_contiguous_bitflag(std::true_type) {
    // constexpr auto& enums = values<E>; // C++14 allows this
    using T = std::underlying_type_t<E>;
    // values<E> is std::array
    // .size() is constexpr
    auto size = values<E>.size();
    if (size == 0) return false;

    // In C++14 loop is allowed in constexpr.
    // We need to know has_zero_flag<E> (which is constexpr variable).
    // Loop from has_zero_flag<E> to size - 1.

    // Wait, values<E> is a constexpr variable, so we can access it.
    for (auto i = std::size_t{has_zero_flag<E>}; i < size - 1; ++i)
      if (T(values<E>[i]) << 1 != T(values<E>[i + 1]))
        return false;
    return true;
  }

  template <typename E>
  constexpr bool check_is_contiguous_bitflag(std::false_type) {
    return false;
  }
}

template<typename E>
static constexpr bool is_contiguous_bitflag = details::check_is_contiguous_bitflag<E>(std::bool_constant<is_bitflag<E>>{});


#ifdef __cpp_concepts
template<typename E>
concept ContiguousEnum = Enum<E> && is_contiguous<E>;
template<typename E>
concept ContiguousBitFlagEnum = BitFlagEnum<E> && is_contiguous_bitflag<E>;
#endif

} // namespace enchantum
