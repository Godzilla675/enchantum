#pragma once

// std::format requires C++20
#if (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L))

#include <format>
#include <string_view>
#include "details/format_util.hpp"
#include "common.hpp"

#ifdef __cpp_concepts
template<enchantum::Enum E>
struct std::formatter<E> : std::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return std::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};
#endif

#endif // C++20 check