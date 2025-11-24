#pragma once


#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

#ifndef ENCHANTUM_ALIAS_STRING_VIEW
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #include <string_view>
  #else
    // C++14 compatibility - provide minimal string_view implementation
    #include "cpp14_string_view.hpp"
  #endif
#endif


namespace enchantum {
#ifdef ENCHANTUM_ALIAS_STRING_VIEW
ENCHANTUM_ALIAS_STRING_VIEW;
#else
  #if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    using ::std::string_view;
  #else
    using ::enchantum::detail::string_view;
  #endif
#endif

} // namespace enchantum