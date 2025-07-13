#pragma once

#ifndef ENCHANTUM_ENABLE_FORMATTING
  #define ENCHANTUM_ENABLE_FORMATTING
#endif

#include "bitflags.hpp"
#include "details/format_util.hpp"
#include "enchantum.hpp"
#include <concepts>
#include <iostream>
#include <string>

namespace enchantum::iostream_operators {
template<typename Traits, Enum E>
std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
{
  return os << details::format(e);
}

template<typename Traits, Enum E>
  requires std::assignable_from<E&, E>
std::basic_istream<char, Traits>& operator>>(std::basic_istream<char, Traits>& is, E& value)
{
  std::basic_string<char, Traits> s;
  is >> s;
  if (!is)
    return is;

  if constexpr (is_bitflag<E>) {
    if (const auto v = enchantum::cast_bitflag<E>(s))
      value = *v;
    else
      is.setstate(std::ios_base::failbit);
  }
  else {
    if (const auto v = enchantum::cast<E>(s))
      value = *v;
    else
      is.setstate(std::ios_base::failbit);
  }
  return is;
}
} // namespace enchantum::iostream_operators
