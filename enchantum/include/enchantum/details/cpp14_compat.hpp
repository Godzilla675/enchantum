#pragma once

// C++14 compatibility layer for enchantum
// Provides C++17 features for C++14 compilers

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
  // C++17 or later - use standard features
  #define ENCHANTUM_HAS_CPP17 1
#else
  // C++14 - provide compatibility implementations
  #define ENCHANTUM_HAS_CPP17 0
#endif

#if !ENCHANTUM_HAS_CPP17

#include <type_traits>

namespace std {

// void_t (C++17 feature backported to C++14)
#if !defined(__cpp_lib_void_t)
template<typename...>
using void_t = void;
#endif

// bool_constant (C++17 feature backported to C++14)
#if !defined(__cpp_lib_bool_constant)
template<bool B>
using bool_constant = integral_constant<bool, B>;
#endif

// Type trait _v helpers (C++17 feature backported to C++14)
#if !defined(__cpp_lib_type_trait_variable_templates)

template<typename T>
constexpr bool is_enum_v = is_enum<T>::value;

template<typename T>
constexpr bool is_void_v = is_void<T>::value;

template<typename T>
constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
constexpr bool is_function_v = is_function<T>::value;

template<typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

template<typename T>
constexpr bool is_array_v = is_array<T>::value;

template<typename T>
constexpr bool is_pointer_v = is_pointer<T>::value;

template<typename T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template<typename T>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template<typename T>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

template<typename T>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

template<typename T>
constexpr bool is_fundamental_v = is_fundamental<T>::value;

template<typename T>
constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

template<typename T>
constexpr bool is_scalar_v = is_scalar<T>::value;

template<typename T>
constexpr bool is_object_v = is_object<T>::value;

template<typename T>
constexpr bool is_compound_v = is_compound<T>::value;

template<typename T>
constexpr bool is_reference_v = is_reference<T>::value;

template<typename T>
constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

template<typename T>
constexpr bool is_const_v = is_const<T>::value;

template<typename T>
constexpr bool is_volatile_v = is_volatile<T>::value;

template<typename T>
constexpr bool is_trivial_v = is_trivial<T>::value;

template<typename T>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

template<typename T>
constexpr bool is_standard_layout_v = is_standard_layout<T>::value;

template<typename T>
constexpr bool is_pod_v = is_pod<T>::value;

template<typename T>
constexpr bool is_literal_type_v = is_literal_type<T>::value;

template<typename T>
constexpr bool is_empty_v = is_empty<T>::value;

template<typename T>
constexpr bool is_polymorphic_v = is_polymorphic<T>::value;

template<typename T>
constexpr bool is_abstract_v = is_abstract<T>::value;

template<typename T>
constexpr bool is_final_v = is_final<T>::value;

template<typename T>
constexpr bool is_signed_v = is_signed<T>::value;

template<typename T>
constexpr bool is_unsigned_v = is_unsigned<T>::value;

template<typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

template<typename T>
constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

template<typename T>
constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

template<typename T>
constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

template<typename T, typename U>
constexpr bool is_assignable_v = is_assignable<T, U>::value;

template<typename T>
constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

template<typename T>
constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

template<typename T>
constexpr bool is_destructible_v = is_destructible<T>::value;

template<typename T, typename... Args>
constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;

template<typename T>
constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;

template<typename T>
constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;

template<typename T>
constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;

template<typename T, typename U>
constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;

template<typename T>
constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;

template<typename T>
constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<T>::value;

template<typename T>
constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

template<typename T, typename... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

template<typename T>
constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;

template<typename T>
constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;

template<typename T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

template<typename T, typename U>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;

template<typename T>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;

template<typename T>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;

template<typename T>
constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

template<typename T>
constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;

template<typename Base, typename Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

template<typename From, typename To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;

template<typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

#endif // !defined(__cpp_lib_type_trait_variable_templates)

// is_invocable (C++17 feature backported to C++14)
#if !defined(__cpp_lib_is_invocable)

namespace detail {
template<typename F, typename... Args>
struct is_invocable_impl {
private:
    template<typename U, typename... UArgs>
    static auto test(int) -> decltype(std::declval<U>()(std::declval<UArgs>()...), std::true_type{});
    
    template<typename, typename...>
    static std::false_type test(...);
    
public:
    using type = decltype(test<F, Args...>(0));
};
} // namespace detail

template<typename F, typename... Args>
struct is_invocable : detail::is_invocable_impl<F, Args...>::type {};

template<typename F, typename... Args>
constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

#endif // !defined(__cpp_lib_is_invocable)

} // namespace std

#endif // !ENCHANTUM_HAS_CPP17

// if constexpr compatibility macro
#if ENCHANTUM_HAS_CPP17
  #define ENCHANTUM_IF_CONSTEXPR if constexpr
  #define ENCHANTUM_INLINE_VAR inline
#else
  #define ENCHANTUM_IF_CONSTEXPR if
  #define ENCHANTUM_INLINE_VAR
  // For C++14: template<auto V> is not supported; manual conversion is required.
#endif
