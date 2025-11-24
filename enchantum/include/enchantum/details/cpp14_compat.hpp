#pragma once

#include <type_traits>

// C++14 compatibility layer for C++17 features

namespace enchantum {
namespace details {

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
// C++17 or later - use standard library features
#define ENCHANTUM_HAS_CPP17 1

// Use standard library _v shortcuts
using enchantum::is_enum_v;
using enchantum::is_convertible_v;
using enchantum::is_same_v;
using enchantum::is_signed_v;

// if constexpr is available
#define ENCHANTUM_IF_CONSTEXPR if constexpr

// inline variables are available
#define ENCHANTUM_INLINE_VAR inline

#else
// C++14 - provide polyfills
#define ENCHANTUM_HAS_CPP17 0

// Provide _v shortcuts for type traits
template<typename T>
constexpr bool is_enum_v = std::is_enum<T>::value;

template<typename From, typename To>
constexpr bool is_convertible_v = std::is_convertible<From, To>::value;

template<typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

template<typename T>
constexpr bool is_signed_v = std::is_signed<T>::value;

// if constexpr is not available, use regular if
#define ENCHANTUM_IF_CONSTEXPR if

// inline variables not available in C++14, need workarounds
#define ENCHANTUM_INLINE_VAR static constexpr

#endif

} // namespace details
} // namespace enchantum
