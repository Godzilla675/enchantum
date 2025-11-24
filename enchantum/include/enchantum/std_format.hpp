#pragma once

#if __cplusplus >= 202002L && __has_include(<format>)

#include <format>
#include <string_view>
#include "details/format_util.hpp"
#include "common.hpp"

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
struct std::formatter<E> : std::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return std::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};

#endif // __cplusplus >= 202002L && __has_include(<format>)