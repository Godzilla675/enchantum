#pragma once

#ifndef ENCHANTUM_ENABLE_FORMATTING
  #define ENCHANTUM_ENABLE_FORMATTING
#endif

#include <format>
#include <string_view>
#include "details/format_util.hpp"
#include "common.hpp"

template<enchantum::Enum E>
struct std::formatter<E> : std::formatter<string_view> {
  template<typename FmtContext>
  constexpr auto format(const E e, FmtContext& ctx) const
  {
    return std::formatter<string_view>::format(enchantum::details::format(e), ctx);
  }
};