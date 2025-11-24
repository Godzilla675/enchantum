#pragma once

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_OPTIONAL
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #include <optional>
  #else
    // C++14 compatibility - provide minimal optional implementation
    #include "cpp14_optional.hpp"
  #endif
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#else
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    using ::std::optional;
  #else
    using ::enchantum::detail::optional;
  #endif
#endif

} // namespace enchantum