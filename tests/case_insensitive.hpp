#pragma once
#include <cstddef>
#include <enchantum/details/string_view.hpp>

struct CaseInsenitive {

  constexpr bool operator()(const char a, const char b) const
  {
    constexpr auto tolower = [](const char c) { return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c; };
    return tolower(a) == tolower(b);
  }
};

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
inline constexpr CaseInsenitive case_insensitive;
#else
static constexpr CaseInsenitive case_insensitive{};
#endif

struct CaseInsenitiveByStrings {
  constexpr bool operator()(const enchantum::string_view a, const enchantum::string_view b) const
  {
    if (a.size() != b.size())
      return false;

    for (std::size_t i = 0; i < a.size(); ++i)
      if (!case_insensitive(a[i], b[i]))
        return false;
    return true;
  }
};

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
inline constexpr CaseInsenitiveByStrings case_insensitive_by_strings;
#else
static constexpr CaseInsenitiveByStrings case_insensitive_by_strings{};
#endif


struct CaseInsenitiveBoth {
  constexpr bool operator()(char a, char b) const { return case_insensitive(a, b); }
  template<typename String>
  constexpr bool operator()(const String& a, const String& b) const
  {
    static_assert(sizeof(String) == 0, "Not chosen");
    return case_insensitive_by_strings(a, b);
  }
};

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
inline constexpr CaseInsenitiveBoth case_insensitive_both;
#else
static constexpr CaseInsenitiveBoth case_insensitive_both{};
#endif
