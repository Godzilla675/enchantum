#pragma once
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

// This macro controls the compile time optimization of msvc
#ifndef ENCHANTUM_ENABLE_MSVC_SPEEDUP
  #define ENCHANTUM_ENABLE_MSVC_SPEEDUP 1
#endif
namespace enchantum {


#define SZC(x) (sizeof(x) - 1)
namespace details {

#if __cplusplus >= 201703L
  template<auto Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
    auto s = string_view{__FUNCSIG__ + SZC("auto __cdecl enchantum::details::enum_in_array_name_size<"),
                         SZC(__FUNCSIG__) - SZC("auto __cdecl enchantum::details::enum_in_array_name_size<>(void) noexcept")};

    if constexpr (is_scoped_enum<decltype(Enum)>) {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0"));
        return s.size();
      }
      return s.substr(0, s.rfind(':') - 1).size();
    }
    else {
      if (s[0] == '(') {
        s.remove_prefix(SZC("(enum "));
        s.remove_suffix(SZC(")0x0"));
      }
      if (const auto pos = s.rfind(':'); pos != s.npos)
        return pos - 1;
      return std::size_t(0);
    }
  }

  template<auto... Vs>
  constexpr auto __cdecl var_name() noexcept
  {
    // MSVC output format search
    const char* p = __FUNCSIG__;
    while (*p != '<' && *p != '\0') p++;
    if (*p == '<') return p + 1;
    return __FUNCSIG__ + SZC("auto __cdecl enchantum::details::var_name<");
  }
#else
  // C++14 support
  template<typename E, E Enum>
  constexpr auto enum_in_array_name_size() noexcept
  {
      // MSVC logic for parsing __FUNCSIG__
      auto s = string_view(__FUNCSIG__);
      // Look for `enum ...`
      auto pos = s.rfind("(enum ");
      if (pos != string_view::npos) {
         s = s.substr(pos + 6);
         pos = s.find(')');
         if (pos != string_view::npos) {
             auto name = s.substr(0, pos);
             auto colon = name.rfind("::");
             if (colon != string_view::npos) return colon + 2; // Include :: ? No, prefix length.
             // If name is Namespace::Enum, rfind returns pos of ::.
             // We want length of Namespace::Enum::.
             return name.size() + 2;
         }
      }
      return std::size_t{0};
  }

  template<typename E, E... Vs>
  constexpr auto __cdecl var_name() noexcept
  {
      const char* p = __FUNCSIG__;
      while (*p != '<' && *p != '\0') p++;
      if (*p == '<') return p + 1;
      return __FUNCSIG__;
  }
#endif

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
    // clang-format off
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
    constexpr auto skip_work_if_neg = IsBitFlag || std::is_unsigned<IntType>::value || sizeof(IntType) <= 2 ? 0 :
#if _MSC_VER <= 1931
        sizeof(IntType) == 4
#else
        std::is_same_v<IntType,char32_t> 
#endif
        ? sizeof(char32_t)*2-1 : sizeof(std::uint64_t)*2-1 - (sizeof(IntType)==8);
#endif
    // clang-format on
    for (std::size_t index = 0; index < array_size; ++index) {
      if (*str == '\0') break;
      while (*str == ' ') str++;
      if (*str == '\0') break;

#if _MSC_VER <= 1924
      if (*str == '0') {
#else
      if (*str == '(') {
#endif
#if ENCHANTUM_ENABLE_MSVC_SPEEDUP
        details::constexpr_if_else<skip_work_if_neg != 0>(
            [&]() {
                const auto i = min + static_cast<IntType>(index);
                auto skip = least_length_when_casting + ((i < 0) * skip_work_if_neg);
                for(size_t k=0; k<skip; ++k) if(*str) str++;
            },
            [&]() {
                 for(size_t k=0; k<least_length_when_casting; ++k) if(*str) str++;
            }
        );
#else
        for(size_t k=0; k<least_length_when_casting; ++k) if(*str) str++;
#endif
        if (*str == '\0') break;
        while (*str != ',' && *str != '\0' && *str != '>') str++;
        if (*str == ',') str++;
      }
      else {
        for(size_t k=0; k<least_length_when_value; ++k) if(*str) str++;
        if (*str == '\0') break;

        details::constexpr_if_else<IsBitFlag>(
            [&]() { values[valid_count] = index == 0 ? IntType{} : static_cast<IntType>(IntType{1} << (index - 1)); },
            [&]() { values[valid_count] = static_cast<IntType>(min + static_cast<IntType>(index)); }
        );

        const char* p = str;
        while (*p != ',' && *p != '\0' && *p != '>') p++;
        std::size_t len = p - str;

        string_lengths[valid_count++] = static_cast<std::uint8_t>(len);
        for(size_t k=0; k<len; ++k) strings[total_string_length++] = str[k];
        total_string_length += null_terminated;

        str = p;
        if (*str == ',') str += SZC(",");
      }
    }
  }

  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  struct reflect_helper_msvc {
      static constexpr auto get() noexcept {
          constexpr auto ArraySize = sizeof...(Is) + is_bitflag<E>;
          using MinT               = decltype(Min);
          using Under              = std::underlying_type_t<E>;
          using Underlying = typename std::make_unsigned<typename std::conditional<std::is_same<bool, Under>::value, unsigned char, Under>::type>::type;
          (void)sizeof(Underlying);

          // MSVC var_name generation
#if __cplusplus >= 201703L
          constexpr auto str = []() {
              if constexpr (is_bitflag<E>)
                   // MSVC logic for bitflag var_name?
                   // Original code: if constexpr (always_true && is_bitflag<E>) return details::var_name<static_cast<E>(!always_true), static_cast<E>(Underlying(1) << Is)..., 0>();
                   // Wait, I need to check original MSVC implementation for bitflags logic in C++17.
                   // Original: if constexpr (always_true && is_bitflag<E>) ...
                   return details::var_name<static_cast<E>(0), static_cast<E>(Underlying(1) << Is)..., 0>();
              else
                   return details::var_name<static_cast<E>(static_cast<MinT>(Is) + Min)..., int(0)>();
          }();
#else
          constexpr auto str = details::constexpr_if_else<is_bitflag<E>>(
              [&]() {
                  return details::var_name<E, static_cast<E>(0), static_cast<E>(Underlying(1) << Is)..., static_cast<E>(0)>();
              },
              [&]() {
                  return details::var_name<E, static_cast<E>(static_cast<MinT>(Is) + Min)..., static_cast<E>(0)>();
              }
          );
#endif

          constexpr auto type_name_len     = details::raw_type_name_func<E>().size() - 1;
#if __cplusplus >= 201703L
          constexpr auto enum_in_array_len = details::enum_in_array_name_size<E{}>();
#else
          constexpr auto enum_in_array_len = details::enum_in_array_name_size<E, E{}>();
#endif

          ReflectStringReturnValue<std::underlying_type_t<E>, ArraySize> ret{};
          details::parse_string<is_bitflag<E>>(
            /*str = */ str,
    #if _MSC_VER <= 1924
            /*least_length_when_casting=*/SZC("0x0"),
    #else
            /*least_length_when_casting=*/SZC("(enum ") + type_name_len + SZC(")0x0") + (sizeof(E) == 8),
    #endif
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
      return details::reflect_helper_msvc<E, NullTerminated, static_cast<std::underlying_type_t<E>>(Min), Is...>::get();
  }
#else
  template<typename E, bool NullTerminated, std::underlying_type_t<E> Min, std::size_t... Is>
  constexpr auto reflect(std::index_sequence<Is...>) noexcept
  {
      return details::reflect_helper_msvc<E, NullTerminated, Min, Is...>::get();
  }
#endif
} // namespace details
} // namespace enchantum

#undef SZC
