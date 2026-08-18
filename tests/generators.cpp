#include "test_utility.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <enchantum/generators.hpp>
#ifdef __cpp_concepts
  #if defined(__has_include) && __has_include(<ranges>)
    #include <ranges>
  #endif
  #include <concepts>
#endif
namespace {
#define ADD_COMP(OP, NAME)                              \
  template<typename T, typename U = T, typename = void> \
  constexpr bool is_##NAME##_comparable = false;        \
  template<typename T, typename U>                      \
  constexpr bool is_##NAME##_comparable<                \
    T,                                                  \
    U,                                                  \
    decltype((void(std::declval<T>() OP std::declval<U>()), void(std::declval<U>() OP std::declval<T>())))> = true


ADD_COMP(==, eq);
ADD_COMP(!=, neq);
ADD_COMP(<, less);
ADD_COMP(>, greater);
ADD_COMP(>=, greater_equal);
ADD_COMP(<=, less_equal);

} // namespace

enum class A {
  a,
  b,
  c
};

using entries = decltype(enchantum::entries_generator<A>);
using values  = decltype(enchantum::values_generator<A>);
using names   = decltype(enchantum::names_generator<A>);

using AllGenerators = type_list<entries, values, names>;

TEMPLATE_LIST_TEST_CASE("generator iterators", "[generators]", AllGenerators)
{
  auto begin = TestType{}.begin();
  auto end   = TestType{}.end();

  using iterator = decltype(begin);
  using senitiel = decltype(end);
#if defined(__cpp_concepts) && defined(__has_include) && __has_include(<ranges>)
  STATIC_CHECK(std::ranges::random_access_range<TestType>);
#endif

  SECTION("comparable")
  {
#ifdef __cpp_concepts
    STATIC_CHECK(std::random_access_iterator<iterator>);
#endif
    STATIC_CHECK(is_eq_comparable<iterator>);
    STATIC_CHECK(is_neq_comparable<iterator>);
    STATIC_CHECK(is_less_comparable<iterator>);
    STATIC_CHECK(is_greater_comparable<iterator>);
    STATIC_CHECK(is_less_equal_comparable<iterator>);
    STATIC_CHECK(is_greater_equal_comparable<iterator>);

    STATIC_CHECK(is_eq_comparable<iterator, senitiel>);
    STATIC_CHECK(is_neq_comparable<iterator, senitiel>);
    STATIC_CHECK(is_less_comparable<iterator, senitiel>);
    STATIC_CHECK(is_greater_comparable<iterator, senitiel>);
    STATIC_CHECK(is_less_equal_comparable<iterator, senitiel>);
    STATIC_CHECK(is_greater_equal_comparable<iterator, senitiel>);
    STATIC_CHECK(std::is_same_v<iterator, senitiel>);
  }

  SECTION("comparisons")
  {
    auto check = [](auto a, auto b) {
      CAPTURE(b - a);
      CHECK(a < b);
      CHECK_FALSE(a > b);
      CHECK(a <= b);
      CHECK_FALSE(a >= b);

      CHECK_FALSE(a == b);
      CHECK(a != b);

      CHECK_FALSE(b < a);
      CHECK(b > a);
      CHECK(a <= b);
      CHECK_FALSE(a >= b);

      CHECK_FALSE(a == b);
      CHECK(a != b);
    };

    check(begin, iterator{} + 1);
    check(begin, end);
  }
}
