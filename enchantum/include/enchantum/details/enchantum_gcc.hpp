#include "../common.hpp"
#include "../type_name.hpp"
#include "shared.hpp"
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "constexpr_if.hpp"

#if defined(__has_include) && __has_include(<bits/char_traits.h>)
  #include <bits/char_traits.h>
#endif
#include "string_view.hpp"

#define ENCAHNTUM_DETAILS_GCC_MAJOR __GNUC__
#if __GNUC__ <= 10
// for out of bounds conversions for C style enums
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace enchantum {
namespace details {
#define SZC(x) (sizeof(x) - 1)


#if __cplusplus >= 201703L
  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
        auto s  = string_view(__PRETTY_FUNCTION__ +
                           SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = "),
                         SZC(__PRETTY_FUNCTION__) -
                           SZC("constexpr auto enchantum::details::enum_in_array_name_size() [with auto Enum = ]"));
        using E = decltype(Enum);

        if constexpr (!std::is_convertible_v<E, std::underlying_type_t<E>>) {
            return s[0] == '(' ? s.size() - SZC("()0") : s.rfind(':') - 1;
        } else {
             if (s[0] == '(') {
                s.remove_prefix(SZC("("));
                s.remove_suffix(SZC(")0"));
              }
              if (const auto pos = s.rfind(':'); pos != string_view::npos)
                return pos - 1;
              return std::size_t{0};
        }
  }

  template<auto... Vs>
  constexpr auto var_name() noexcept
  {
    const char* p = __PRETTY_FUNCTION__;
    while (*p != '{' && *p != '\0') p++;
    if (*p == '{') return p + 1;
    return __PRETTY_FUNCTION__ + SZC("constexpr auto enchantum::details::var_name() [with auto ...Vs = {");
  }
#else
  // C++14 support

  template<typename E, E Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
      auto s = string_view(__PRETTY_FUNCTION__);
      auto pos = s.rfind(" = ");
      if (pos != string_view::npos) {
          s = s.substr(pos + 3);
          if (!s.empty() && s[s.size()-1] == ']') s.remove_suffix(1);
          auto colon = s.rfind(':');
          if (colon != string_view::npos) {
              return colon - 1;
          }
      }
      return std::size_t{0};
  }

  // GCC 10 workaround
  template<typename E, E V>
  constexpr auto gcc10_workaround() noexcept {
     return std::size_t{0};
  }

  template<typename E, E... Vs>
  constexpr auto var_name() noexcept
  {
      const char* p = __PRETTY_FUNCTION__;
      while (*p != '{' && *p != '\0') p++;
      if (*p == '{') return p + 1;
      return __PRETTY_FUNCTION__;
  }
#endif

  template<typename Enum>
  constexpr auto length_of_enum_in_template_array_if_casting() noexcept
  {
    // Use constexpr_if_else
    return details::constexpr_if_else<is_scoped_enum<Enum>>(
        [&]() {
#if __cplusplus >= 201703L
            return details::enum_in_array_name_size<Enum{}>();
#else
            return details::enum_in_array_name_size<Enum, Enum{}>();
#endif
        },
        [&]() {
            return std::size_t{0};
        }
    );
  }

  template<bool IsBitFlag, typename IntType>
  constexpr void parse_string(
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
    (void)min; // not always used
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '\0') break;

      // Skip whitespace
      while (*str == ' ') str++;
      if (*str == '\0') break;

      // Check for cast or number (skip)
      if (*str == '(' || *str == '-' || (*str >= '0' && *str <= '9')) {
        // Skip logic
        if (*str == '(') {
            // Safely skip
            for(std::size_t k=0; k<least_length_when_casting; ++k) {
                if (*str == '\0') break;
                str++;
            }
        }
        if (*str == '\0') break;

        const char* p = str;
        while (*p != ',' && *p != '\0') p++;
        if (*p == '\0') {
             str = p;
             break;
        }
        // Safer advance
        str = p;
        if (*str == ',') {
            str++;
            if (*str == ' ') str++;
        }
      }
      else {
        // Safely advance least_length_when_value
        for(std::size_t k=0; k<least_length_when_value; ++k) {
            if (*str == '\0') break;
            str++;
        }
        if (*str == '\0') break;

        const char* p = str;
        while (*p != ',' && *p != '\0' && *p != '}') p++;

        const auto commapos = static_cast<std::size_t>(p - str);

        details::constexpr_if_else<IsBitFlag>(
            [&]() { values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1)); },
            [&]() { values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index)); }
        );

        string_lengths[valid_count++] = static_cast<std::uint8_t>(commapos);
        for (std::size_t i = 0; i < commapos; ++i)
          strings[total_string_length++] = str[i];
        total_string_length += null_terminated;

        if (*p == '}') {
             str = p;
             break;
        }
        else if (*p == '\0') {
             str = p;
             break;
        }
        else {
             str = p; // points to comma
             if (*str == ',') {
                 str++;
                 if (*str == ' ') str++;
             }
        }
      }
    }
  }

  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  struct reflect_helper {
      static constexpr auto get() noexcept {
          constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
          using Under              = std::underlying_type_t<E>;
          using Underlying = typename std::make_unsigned<typename std::conditional<std::is_same<bool, Under>::value, unsigned char, Under>::type>::type;
          (void)sizeof(Underlying);

#if __GNUC__ <= 10
      #define CAST(type, value) static_cast<type>(value)
#else
      #define CAST(type, value) __builtin_bit_cast(type, value)
#endif
          // C++14 string generation
#if __cplusplus >= 201703L
          constexpr auto str = []() {
              if constexpr (is_bitflag<E>)
                  return details::var_name<E{}, CAST(E, static_cast<Under>(Underlying{1} << Is))..., 0>();
              else
                  return details::var_name<CAST(E, static_cast<Under>(static_cast<decltype(Min)>(Is) + Min))..., 0>();
          }();
#else
          constexpr auto str = details::constexpr_if_else<is_bitflag<E>>(
              [&]() {
                 return details::var_name<E, CAST(E, static_cast<Under>(Underlying{1} << Is))..., CAST(E, 0)>();
              },
              [&]() {
                 return details::var_name<E, CAST(E, static_cast<Under>(static_cast<Under>(Is) + Min))..., CAST(E, 0)>();
              }
          );
#endif
#undef CAST

#if __cplusplus >= 201703L
          constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
#else
          constexpr auto enum_in_array_len = details::enum_in_array_name_size<E, E{}>();
#endif
          constexpr auto length_of_enum_in_template_array_casting = details::length_of_enum_in_template_array_if_casting<E>();

          ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret{};
          details::parse_string<is_bitflag<E>>(
            /*str = */ str,
            /*least_length_when_casting=*/SZC("(") + length_of_enum_in_template_array_casting + SZC(")0"),
            /*least_length_when_value=*/details::prefix_length_or_zero<E> +
              (enum_in_array_len != 0 ? enum_in_array_len + SZC("::") : 0),
            /*min = */ static_cast<std::underlying_type_t<E>>(Min),
            /*array_size = */ ArraySize,
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
      return details::reflect_helper<E, NullTerminated, static_cast<std::underlying_type_t<E>>(Min), Is...>::get();
  }
#else
  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
      return details::reflect_helper<E, NullTerminated, Min, Is...>::get();
  }
#endif

} // namespace details

} // namespace enchantum

#undef SZC

#if __GNUC__ <= 10
  #pragma GCC diagnostic pop
#endif
