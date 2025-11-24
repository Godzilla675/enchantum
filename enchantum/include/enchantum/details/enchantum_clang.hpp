#pragma once

// Clang <= 12 outputs "NUMBER" if casting
// Clang > 12 outputs "(E)NUMBER".

#if defined __has_warning
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
  #endif
#endif

#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include "string_view.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>
#include "constexpr_if.hpp"

namespace enchantum {

namespace details {
  constexpr auto enum_in_array_name(const string_view raw_type_name, const bool is_scoped_enum) noexcept
  {
    if (is_scoped_enum)
      return raw_type_name;

    if (const auto pos = raw_type_name.rfind(':'); pos != string_view::npos)
      return raw_type_name.substr(0, pos - 1);
    return string_view();
  }

#define SZC(x) (sizeof(x) - 1)

#if __cplusplus >= 201703L
  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    // "auto enchantum::details::var_name() [Vs = <(A)0, a, b, c, e, d, (A)6>]"
    return __PRETTY_FUNCTION__ + SZC("auto enchantum::details::var_name() [Vs = <");
  }
#else
  // C++14 support
  // We need to check what Clang outputs in C++14 mode.
  // It likely outputs [E = ..., Vs = <...>] or similar.
  template<typename E, E... Vs>
  constexpr auto var_name() noexcept
  {
      // Fallback: search for '<' which starts the template args list?
      // Or search for '{' if it uses list init?
      // Clang usually outputs template args in [ ... ].
      const char* p = __PRETTY_FUNCTION__;
      // Look for start of values.
      // Clang often uses `Vs = <...>` or `...Vs = <...>`.
      // Let's search for `<`.
      while (*p != '<' && *p != '\0') p++;
      if (*p == '<') return p + 1;
      return __PRETTY_FUNCTION__;
  }
#endif

  template<bool IsBitFlag, typename IntType>
  constexpr void parse_string(
    std::size_t         index_check,
    const char*         str,
    const std::size_t   least_length_when_casting,
    const std::size_t   least_length_when_value,
    const IntType       min,
    const std::size_t   array_size,
    const bool          null_terminated,
    IntType* const      values,
    std::uint8_t* const string_lengths,
    char* const         strings,
    std::size_t&        total_string_length,
    std::size_t&        valid_count)
  {
    (void)index_check;
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '\0') break;

      // Clang might have different spacing.
      while (*str == ' ') str++;
      if (*str == '\0') break;

#if __clang_major__ > 12
      // check if cast (starts with '(')
      if (str[index_check] == '(')
#else
      // check if it is a number or negative sign
      if (str[0] == '-' || (str[0] >= '0' && str[0] <= '9'))
#endif
      {
        str = __builtin_char_memchr(str + least_length_when_casting, ',', UINT8_MAX);
        if (!str) break; // Not found
        str += SZC(", ");
      }
      else {
        str += least_length_when_value;
        if (*str == '\0') break;

        const char* end = __builtin_char_memchr(str, ',', UINT8_MAX);
        const auto commapos = end ? static_cast<std::size_t>(end - str) : string_view(str).size(); // If no comma, take rest?
        // Clang list ends with >?
        const char* p = str;
        while (*p != ',' && *p != '>' && *p != '\0') p++;
        const std::size_t len = p - str;

        details::constexpr_if_else<IsBitFlag>(
            [&]() { values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1)); },
            [&]() { values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index)); }
        );

        string_lengths[valid_count++] = static_cast<std::uint8_t>(len);
        __builtin_memcpy(strings + total_string_length, str, len);
        total_string_length += len + null_terminated;

        if (*p == '>') { // End of list
            str = p;
            break;
        }
        str = p;
        if (*str == ',') str += SZC(", ");
      }
    }
  }
  
  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  struct reflect_helper_clang {
      static constexpr auto get() noexcept {
        using MinT       = decltype(Min);
        using T          = std::underlying_type_t<E>;
        using Underlying = typename std::make_unsigned<typename std::conditional<std::is_same<bool, T>::value, unsigned char, T>::type>::type;
        (void)sizeof(Underlying);

#if __cplusplus >= 201703L
        constexpr auto str = []() {
            constexpr bool always_true = sizeof...(Is) != -1; // Always true dummy
             if constexpr (always_true && is_bitflag<E>)
                return details::var_name<static_cast<E>(!always_true), static_cast<E>(Underlying(1) << Is)..., 0>();
             else
                return details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., int(!always_true)>();
        }();
#else
        // C++14
        constexpr auto str = details::constexpr_if_else<is_bitflag<E>>(
            [&]() {
                 return details::var_name<E, static_cast<E>(0), static_cast<E>(Underlying(1) << Is)..., static_cast<E>(0)>();
            },
            [&]() {
                 return details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., static_cast<E>(0)>();
            }
        );
#endif

        constexpr auto enum_in_array_name_val = details::enum_in_array_name(raw_type_name<E>, is_scoped_enum<E>);
        constexpr auto enum_in_array_len  = enum_in_array_name_val.size();

        ReflectStringReturnValue<std::underlying_type_t<E>, sizeof...(Is) + is_bitflag<E>> ret{};

        constexpr std::size_t index_check = enum_in_array_name_val.size() != 0 && enum_in_array_name_val[0] == '(' ? 1 : 0;

        details::parse_string<is_bitflag<E>>(
            /*index_check=*/index_check,
            /*str = */ str,
    #if __clang_major__ > 12
            /*least_length_when_casting=*/SZC("(") + enum_in_array_len + SZC(")0"),
    #else
            /*least_length_when_casting=*/1,
    #endif
            /*least_length_when_value=*/details::prefix_length_or_zero<E> +
            (enum_in_array_len != 0 ? enum_in_array_len + SZC("::") : 0),
            /*min = */ static_cast<T>(Min),
            /*array_size = */ sizeof...(Is) + is_bitflag<E>,
            /*null_terminated= */ NullTerminated,
            /*enum_values= */ ret.values,
            /*string_lengths= */ ret.string_lengths,
            /*strings= */ ret.strings,
            /*total_string_length*/ ret.total_string_length,
            /*valid_count*/ ret.valid_count);

        return ret;
      }
  };

#if __cplusplus >= 201703L
  template<typename E, bool NullTerminated, auto Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
      return details::reflect_helper_clang<E, NullTerminated, static_cast<std::underlying_type_t<E>>(Min), Is...>::get();
  }
#else
  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
      return details::reflect_helper_clang<E, NullTerminated, Min, Is...>::get();
  }
#endif

} // namespace details

} // namespace enchantum

#if defined __has_warning
  #if __has_warning("-Wenum-constexpr-conversion")
    #pragma clang diagnostic pop
  #endif
#endif
#undef SZC
