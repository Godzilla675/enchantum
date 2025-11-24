#pragma once

#include "bitflags.hpp"
#include "common.hpp"
#include "details/string.hpp"
#include "details/string_view.hpp"
#include "enchantum.hpp"
#include "type_name.hpp"


#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

// Define nodiscard macro if not available
#if __cplusplus < 201703L
  #if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
      #define ENCHANTUM_NODISCARD [[nodiscard]]
    #else
      #define ENCHANTUM_NODISCARD
    #endif
  #else
    #define ENCHANTUM_NODISCARD
  #endif
#else
  #define ENCHANTUM_NODISCARD [[nodiscard]]
#endif


namespace enchantum {
namespace scoped {
  namespace details {

    // Helper for C++14 generic lambda simulation
    struct StartsWith {
      constexpr bool operator()(string_view a, string_view b) const {
          return a.substr(0, b.size()) == b;
      }
    };

    constexpr string_view remove_scope_or_empty(string_view string, const string_view type_name) noexcept
    {
      const StartsWith starts_with{};
      if (!starts_with(string, type_name))
        return string_view();
      string.remove_prefix(type_name.size());
      if (!starts_with(string, string_view("::", 2)))
        return string_view();
      string.remove_prefix(2);
      return string;
    }
  } // namespace details

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  ENCHANTUM_NODISCARD constexpr bool contains(const string_view name) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n);
  }

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename BinaryPredicate>
  ENCHANTUM_NODISCARD constexpr bool contains(const string_view name, const BinaryPredicate binary_predicate) noexcept
  {
    const auto n = details::remove_scope_or_empty(name, type_name<E>);
    return !n.empty() && enchantum::contains<E>(n, binary_predicate);
  }

  namespace details {
    template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
    struct scoped_cast_functor {
      ENCHANTUM_NODISCARD constexpr optional<E> operator()(const string_view name) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : cast<E>(n);
      }

      template<typename BinaryPred>
      ENCHANTUM_NODISCARD constexpr optional<E> operator()(const string_view name, const BinaryPred binary_predicate) const noexcept
      {
        const auto n = details::remove_scope_or_empty(name, type_name<E>);
        return n.empty() ? optional<E>() : cast<E>(n, binary_predicate);
      }
    };

    struct to_scoped_string_functor {
      // hacky workaround about string not being a literal type.
      template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E), typename String = string>
      ENCHANTUM_NODISCARD constexpr String operator()(const E value) const noexcept
      {
        String s;
        const auto i = enchantum::enum_to_index(value);
        if (i) {
          s += type_name<E>;
          s += "::";
          s += names_generator<E>[*i];
          return s;
        }
        return s;
      }
    };
  } // namespace details


  static constexpr details::to_scoped_string_functor to_string{};

  template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
  static constexpr details::scoped_cast_functor<E> cast{};

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
  ENCHANTUM_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
  {
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != string_view::npos; i = s.find(sep, pos)) {
      if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos), binary_pred))
        return false;
      pos = i + 1;
    }
    return enchantum::scoped::contains<E>(s.substr(pos), binary_pred);
  }

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  ENCHANTUM_NODISCARD constexpr bool contains_bitflag(const string_view s, const char sep = '|') noexcept
  {
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != string_view::npos; i = s.find(sep, pos)) {
      if (!enchantum::scoped::contains<E>(s.substr(pos, i - pos)))
        return false;
      pos = i + 1;
    }
    return enchantum::scoped::contains<E>(s.substr(pos));
  }


  template<typename String = string, ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  ENCHANTUM_NODISCARD constexpr String to_string_bitflag(const E value, const char sep = '|')
  {
    using T = std::underlying_type_t<E>;

    // Regular if (has_zero_flag<E>) is fine for logic flow if types are compatible.
    if (has_zero_flag<E>) {
      if (static_cast<T>(value) == 0)
        return enchantum::scoped::to_string(value);
    }

    String         name;
    T              check_value = 0;
    constexpr auto scope_name  = type_name<E>;
    for (auto i = std::size_t{has_zero_flag<E>}; i < count<E>; ++i) {
      const auto v = static_cast<T>(values_generator<E>[i]);
      if (v == (static_cast<T>(value) & v)) {
        if (!name.empty())
          name.append(1, sep); // append separator if not the first value
        name.append(scope_name.data(), scope_name.size());
        name.append("::", 2);
        const auto s = names_generator<E>[i];
        name.append(s.data(), s.size()); // not using operator += since this may not be std::string_view always
        check_value |= v;
      }
    }
    if (check_value == static_cast<T>(value))
      return name;
    return string();
  }


  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E), typename BinaryPred>
  ENCHANTUM_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep, const BinaryPred binary_pred) noexcept
  {
    using T = std::underlying_type_t<E>;
    T           check_value{};
    std::size_t pos = 0;
    for (std::size_t i = s.find(sep); i != string_view::npos; i = s.find(sep, pos)) {
      const auto v = enchantum::scoped::cast<E>(s.substr(pos, i - pos), binary_pred);
      if (v)
        check_value |= static_cast<T>(*v);
      else
        return optional<E>();
      pos = i + 1;
    }

    const auto v = enchantum::scoped::cast<E>(s.substr(pos), binary_pred);
    if (v)
      return optional<E>(static_cast<E>(check_value | static_cast<T>(*v)));
    return optional<E>();
  }

  template<ENCHANTUM_DETAILS_ENUM_BITFLAG_CONCEPT(E)>
  ENCHANTUM_NODISCARD constexpr optional<E> cast_bitflag(const string_view s, const char sep = '|') noexcept
  {
    // C++14 generic lambda
    return enchantum::scoped::cast_bitflag<E>(s, sep, [](const auto& a, const auto& b) { return a == b; });
  }
} // namespace scoped
} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
