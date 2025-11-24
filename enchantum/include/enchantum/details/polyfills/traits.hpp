#pragma once

#include <type_traits>
#if __cplusplus >= 201703L
#include <functional>
#endif

namespace enchantum::details {

#if __cplusplus >= 201703L
using std::void_t;
using std::bool_constant;
using std::invoke;
using std::is_invocable;
using std::is_invocable_v;
#else

template <typename...>
using void_t = void;

template <bool B>
using bool_constant = std::integral_constant<bool, B>;

// std::invoke implementation (partial)
template <typename F, typename... Args>
constexpr auto invoke(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...)) {
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

// std::is_invocable implementation
template <typename F, typename... Args>
struct is_invocable {
private:
    template <typename U, typename... UArgs>
    static auto test(int) -> decltype(std::declval<U>()(std::declval<UArgs>()...), std::true_type{});

    template <typename, typename...>
    static std::false_type test(...);

public:
    static constexpr bool value = decltype(test<F, Args...>(0))::value;
};

template <typename F, typename... Args>
constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

#endif

} // namespace enchantum::details
