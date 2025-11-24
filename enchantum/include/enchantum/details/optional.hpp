#pragma once

#ifdef ENCHANTUM_CONFIG_FILE
  #include ENCHANTUM_CONFIG_FILE
#endif

namespace enchantum {

#ifdef ENCHANTUM_ALIAS_OPTIONAL
ENCHANTUM_ALIAS_OPTIONAL;
#elif __cplusplus >= 201703L
  #include <optional>
  using ::std::optional;
#else
  // Minimal C++14 optional polyfill
  // This implementation assumes T is default constructible, which is true for Enums and size_t used in this library.
  // This allows fully constexpr implementation without unions or placement new in C++14.
  #include <type_traits>
  #include <utility>
  #include <stdexcept>

  template<typename T>
  class optional {
  public:
      constexpr optional() noexcept : has_value_(false), value_{} {}
      constexpr optional(const T& value) : has_value_(true), value_(value) {}
      constexpr optional(T&& value) : has_value_(true), value_(std::move(value)) {}

      constexpr optional(const optional& other) = default;
      constexpr optional(optional&& other) = default;

      // Destructor is trivial if T is trivial
      ~optional() = default;

      optional& operator=(const optional& other) = default;
      optional& operator=(optional&& other) = default;

      constexpr const T* operator->() const noexcept { return &value_; }
      constexpr T* operator->() noexcept { return &value_; }
      constexpr const T& operator*() const noexcept { return value_; }
      constexpr T& operator*() noexcept { return value_; }

      constexpr explicit operator bool() const noexcept { return has_value_; }
      constexpr bool has_value() const noexcept { return has_value_; }

      constexpr T& value() & {
          if (!has_value_) throw std::runtime_error("optional::value");
          return value_;
      }

      constexpr const T& value() const & {
          if (!has_value_) throw std::runtime_error("optional::value");
          return value_;
      }

  private:
      bool has_value_;
      T value_;
  };
#endif

} // namespace enchantum
