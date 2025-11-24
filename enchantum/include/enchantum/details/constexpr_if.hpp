#pragma once

#include <type_traits>

namespace enchantum::details {

#if __cplusplus >= 201703L
  template <bool Cond, typename F, typename... Args>
  constexpr decltype(auto) constexpr_if(F&& f, Args&&... args) {
    if constexpr (Cond) {
      return std::forward<F>(f)(std::forward<Args>(args)...);
    }
  }

  template <bool Cond, typename T, typename F>
  constexpr decltype(auto) constexpr_if_else(T&& true_branch, F&& false_branch) {
      if constexpr (Cond) {
          return std::forward<T>(true_branch)();
      } else {
          return std::forward<F>(false_branch)();
      }
  }
#else
  // C++14 implementation using SFINAE or tag dispatching helper
  // This is tricky because we want to defer compilation of the branch that is not taken.
  // The best way in C++14 is usually to use a lambda with `auto` argument (generic lambda)
  // or a template function.

  template <bool Cond, typename T, typename F>
  constexpr auto constexpr_if_else(T&& true_branch, F&& false_branch)
    -> typename std::enable_if<Cond, decltype(std::forward<T>(true_branch)())>::type
  {
      return std::forward<T>(true_branch)();
  }

  template <bool Cond, typename T, typename F>
  constexpr auto constexpr_if_else(T&& true_branch, F&& false_branch)
    -> typename std::enable_if<!Cond, decltype(std::forward<F>(false_branch)())>::type
  {
      return std::forward<F>(false_branch)();
  }
#endif

} // namespace enchantum::details
