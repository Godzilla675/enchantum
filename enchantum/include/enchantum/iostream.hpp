#pragma once

#include "bitflags.hpp"
#include "details/format_util.hpp"
#include "enchantum.hpp"
#include <iostream>
#include <string>

namespace enchantum {
namespace iostream_operators {
  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits>& os, const E e)
  {
    return os << details::format(e);
  }

  namespace details {
      template<typename E, typename Traits>
      void read_enum(std::basic_istream<char, Traits>& is, E& value, std::basic_string<char, Traits>& s, std::true_type /*is_bitflag*/) {
          if (const auto v = enchantum::cast_bitflag<E>(s))
            value = *v;
          else
            is.setstate(std::ios_base::failbit);
      }

      template<typename E, typename Traits>
      void read_enum(std::basic_istream<char, Traits>& is, E& value, std::basic_string<char, Traits>& s, std::false_type /*is_bitflag*/) {
          if (const auto v = enchantum::cast<E>(s))
            value = *v;
          else
            is.setstate(std::ios_base::failbit);
      }
  }

  template<typename Traits, ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  auto operator>>(std::basic_istream<char, Traits>& is, E& value) -> decltype((value = E{}, is))
  // sfinae to check whether value is assignable
  {
    std::basic_string<char, Traits> s;
    is >> s;
    if (!is)
      return is;

    details::read_enum(is, value, s, std::bool_constant<is_bitflag<E>>{});

    return is;
  }
} // namespace iostream_operators
} // namespace enchantum
