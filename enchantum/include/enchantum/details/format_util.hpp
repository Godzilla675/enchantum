#pragma once

#include "../bitflags.hpp"
#include "../enchantum.hpp"
#include <string>

namespace enchantum {
namespace details {

  // Tag dispatch helpers for format
  template<typename E>
  std::string format_bitflag(E e, std::true_type /*is_same_string*/) {
      const auto name = enchantum::to_string_bitflag(e);
      if (!name.empty())
          return name;
      return std::to_string(+enchantum::to_underlying(e));
  }

  template<typename E>
  std::string format_bitflag(E e, std::false_type /*is_same_string*/) {
      const auto name = enchantum::to_string_bitflag(e);
      if (!name.empty())
          return std::string(name.data(), name.size());
      return std::to_string(+enchantum::to_underlying(e));
  }

  template<typename E>
  std::string format_impl(E e, std::true_type /*is_bitflag*/) {
      return format_bitflag(e, std::is_same<std::string, string>{});
  }

  template<typename E>
  std::string format_impl(E e, std::false_type /*is_bitflag*/) {
      const auto name = enchantum::to_string(e);
      if (!name.empty())
        return std::string(name.data(), name.size());
      return std::to_string(+enchantum::to_underlying(e));
  }

  template<typename E>
  std::string format(E e) noexcept
  {
      return format_impl(e, std::bool_constant<is_bitflag<E>>{});
  }
} // namespace details
} // namespace enchantum
