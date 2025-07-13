#pragma once

// Precompiled header for enchantum library
// This header includes commonly used standard library headers
// that are used throughout the enchantum library

// Core C++ standard library headers used by enchantum
#include <array>
#include <bit>
#include <climits>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

// Optional heavy headers (conditionally included)
#ifdef ENCHANTUM_ENABLE_IOSTREAM
  #include <iostream>
  #include <string>
#endif

#ifdef ENCHANTUM_ENABLE_FORMATTING
  #include <string>
  #if __has_include(<format>)
    #include <format>
  #endif
#endif

#ifndef ENCHANTUM_NO_EXCEPTIONS
  #include <stdexcept>
#endif

#ifndef ENCHANTUM_ALIAS_BITSET
  #include <bitset>
#endif