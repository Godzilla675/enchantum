#pragma once

#include <cstddef>
#include <type_traits>

namespace enchantum {

// Forward declarations for core types
template<typename T>
concept Enum = std::is_enum_v<T>;

// Core function objects (forward declarations)
namespace details {
  struct to_string_functor;
  struct enum_to_index_functor;
  template<typename E> struct index_to_enum_functor;
  template<typename E> struct cast_functor;
}

// Core constexpr function objects
template<Enum E>
extern const details::index_to_enum_functor<E> index_to_enum;

extern const details::enum_to_index_functor enum_to_index;

template<Enum E>
extern const details::cast_functor<E> cast;

extern const details::to_string_functor to_string;

// Core reflection data (forward declarations)
template<Enum E>
extern const std::underlying_type_t<E> min;

template<Enum E>
extern const std::underlying_type_t<E> max;

template<Enum E>
extern const std::size_t count;

} // namespace enchantum