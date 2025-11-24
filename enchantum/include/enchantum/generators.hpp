#pragma once
#include "entries.hpp"
#ifdef __cpp_impl_three_way_comparison
  #include <compare>
#endif
#include "details/countr_zero.hpp"
#include "details/constexpr_if.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  // false positives from T += T
  // it does not make sense.
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wconversion"
#endif

// Define nodiscard macro if not available
#if __cplusplus < 201703L
  #if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(nodiscard)
      #define ENCHANTUM_NODISCARD [[nodiscard]]
    #else
      #define ENCHANTUM_NODISCARD
    #endif
  #else
    #define ENCHANTUM_NODISCARD
  #endif
#else
  #define ENCHANTUM_NODISCARD [[nodiscard]]
#endif

namespace enchantum {
namespace details {

  struct senitiel {};


  template<typename CRTP, std::ptrdiff_t Size>
  struct sized_iterator {
    static_assert(Size < INT16_MAX, "Too many enum entries");
  private:
    using IndexType = std::conditional_t<(Size <= INT8_MAX), std::int8_t, std::int16_t>;
  public:
    IndexType       index{};
    constexpr CRTP& operator+=(const std::ptrdiff_t offset) & noexcept
    {
      index += static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator-=(const std::ptrdiff_t offset) & noexcept
    {
      index -= static_cast<IndexType>(offset);
      return static_cast<CRTP&>(*this);
    }

    constexpr CRTP& operator++() & noexcept
    {
      ++index;
      return static_cast<CRTP&>(*this);
    }
    constexpr CRTP& operator--() & noexcept
    {
      --index;
      return static_cast<CRTP&>(*this);
    }

    ENCHANTUM_NODISCARD constexpr CRTP operator++(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      ++*this;
      return copy;
    }
    ENCHANTUM_NODISCARD constexpr CRTP operator--(int) & noexcept
    {
      auto copy = static_cast<CRTP&>(*this);
      --*this;
      return copy;
    }

    // Remove ENCHANTUM_NODISCARD from friend functions defined inline to avoid GCC warnings/errors about "used but never defined"
    // It seems GCC treats friend definition with attribute weirdly in some versions.
    friend constexpr CRTP operator+(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it += offset;
      return it;
    }

    friend constexpr CRTP operator+(const std::ptrdiff_t offset, CRTP it) noexcept
    {
      it += offset;
      return it;
    }

    friend constexpr CRTP operator-(CRTP it, const std::ptrdiff_t offset) noexcept
    {
      it -= offset;
      return it;
    }

    ENCHANTUM_NODISCARD constexpr std::ptrdiff_t operator-(const sized_iterator that) const noexcept
    {
      return index - that.index;
    }

    ENCHANTUM_NODISCARD constexpr std::ptrdiff_t        operator-(senitiel) const noexcept { return index - Size; }
    friend constexpr std::ptrdiff_t operator-(senitiel, sized_iterator it) noexcept
    {
      return Size - it.index;
    }

    ENCHANTUM_NODISCARD constexpr bool operator==(const sized_iterator that) const noexcept { return that.index == index; };
    ENCHANTUM_NODISCARD constexpr bool operator==(senitiel) const noexcept { return Size == index; }

#ifdef __cpp_impl_three_way_comparison
    ENCHANTUM_NODISCARD constexpr auto operator<=>(const sized_iterator that) const noexcept { return index <=> that.index; };
    ENCHANTUM_NODISCARD constexpr auto operator<=>(senitiel) const noexcept { return index <=> Size; }
#else

    ENCHANTUM_NODISCARD constexpr bool operator!=(const sized_iterator that) const noexcept { return that.index != index; };
    ENCHANTUM_NODISCARD constexpr bool operator!=(senitiel) const noexcept { return Size != index; }

    friend constexpr bool operator==(senitiel, const sized_iterator it) noexcept
    {
      return Size == it.index;
    }


    friend constexpr bool operator!=(senitiel, const sized_iterator it) noexcept
    {
      return Size != it.index;
    }


    ENCHANTUM_NODISCARD constexpr bool operator<(const sized_iterator that) const noexcept { return index < that.index; };
    ENCHANTUM_NODISCARD constexpr bool operator>(const sized_iterator that) const noexcept { return index > that.index; };
    ENCHANTUM_NODISCARD constexpr bool operator<=(const sized_iterator that) const noexcept { return index <= that.index; };
    ENCHANTUM_NODISCARD constexpr bool operator>=(const sized_iterator that) const noexcept { return index >= that.index; };

    ENCHANTUM_NODISCARD constexpr bool operator<(senitiel) const noexcept { return index < Size; };
    ENCHANTUM_NODISCARD constexpr bool operator>(senitiel) const noexcept { return index > Size; };
    ENCHANTUM_NODISCARD constexpr bool operator<=(senitiel) const noexcept { return index <= Size; };
    ENCHANTUM_NODISCARD constexpr bool operator>=(senitiel) const noexcept { return index >= Size; };

    friend constexpr bool operator<(senitiel, const sized_iterator it) noexcept
    {
      return Size < it.index;
    };
    friend constexpr bool operator>(senitiel, const sized_iterator it) noexcept
    {
      return Size > it.index;
    };
    friend constexpr bool operator<=(senitiel, const sized_iterator it) noexcept
    {
      return Size <= it.index;
    };
    friend constexpr bool operator>=(senitiel, const sized_iterator it) noexcept
    {
      return Size >= it.index;
    };

#endif
  };

  template<typename E, typename String = string_view, bool NullTerminated = true>
  struct names_generator_t {
    ENCHANTUM_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = String;
      ENCHANTUM_NODISCARD constexpr String operator*() const noexcept
      {
        const auto* const p       = details::reflection_string_indices<E, NullTerminated>.data();
        const auto* const strings = details::reflection_data_string_storage<E, NullTerminated>;
        return String(strings + p[this->index], p[this->index + 1] - p[this->index] - NullTerminated);
      }

      ENCHANTUM_NODISCARD constexpr String operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    ENCHANTUM_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E>
  struct values_generator_t {
    ENCHANTUM_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = E;

      // Tag dispatching helpers to replace if constexpr
      constexpr E get_value(std::true_type /*is_contiguous*/) const noexcept {
          using T = std::underlying_type_t<E>;
          return static_cast<E>(static_cast<T>(min<E>) + static_cast<T>(this->index));
      }

      constexpr E get_value(std::false_type /*is_contiguous*/) const noexcept {
          return get_value_bitflag(std::bool_constant<is_contiguous_bitflag<E>>{});
      }

      constexpr E get_value_bitflag(std::true_type /*is_contiguous_bitflag*/) const noexcept {
          using T = std::underlying_type_t<E>;
          using UT = typename std::make_unsigned<T>::type;

          constexpr auto real_min_offset = details::countr_zero(static_cast<UT>(values<E>[has_zero_flag<E>]));

          if (has_zero_flag<E>) {
              if (this->index == 0)
                  return E{};
          }

          return static_cast<E>(UT{1} << (real_min_offset + static_cast<UT>(this->index - has_zero_flag<E>)));
      }

      constexpr E get_value_bitflag(std::false_type /*is_contiguous_bitflag*/) const noexcept {
          return values<E>[static_cast<std::size_t>(this->index)];
      }

      ENCHANTUM_NODISCARD constexpr E operator*() const noexcept
      {
        return get_value(std::bool_constant<is_contiguous<E>>{});
      }
      ENCHANTUM_NODISCARD constexpr E operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    ENCHANTUM_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

  template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
  struct entries_generator_t {
    ENCHANTUM_NODISCARD static constexpr std::size_t size() noexcept { return count<E>; }

    struct iterator : sized_iterator<iterator, static_cast<std::ptrdiff_t>(size())> {
      using value_type = Pair;
      ENCHANTUM_NODISCARD constexpr Pair operator*() const noexcept
      {
        return Pair{
          values_generator_t<E>{}[static_cast<std::size_t>(this->index)],
          names_generator_t<E, string_view, NullTerminated>{}[static_cast<std::size_t>(this->index)],
        };
      }
      ENCHANTUM_NODISCARD constexpr Pair operator[](const std::ptrdiff_t i) const noexcept { return *(*this + i); }
    };

    ENCHANTUM_NODISCARD static constexpr auto begin() { return iterator{}; }
    ENCHANTUM_NODISCARD static constexpr auto end() { return senitiel{}; }

    ENCHANTUM_NODISCARD constexpr auto operator[](const std::size_t i) const noexcept
    {
      return *(begin() + static_cast<std::ptrdiff_t>(i));
    }
  };

} // namespace details

template<ENCHANTUM_DETAILS_ENUM_CONCEPT(E)>
static constexpr details::values_generator_t<E> values_generator{};

#ifdef __cpp_concepts
template<Enum E, typename StringView = string_view, bool NullTerminated = true>
static constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<Enum E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true>
static constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#else
template<typename E, typename StringView = string_view, bool NullTerminated = true, std::enable_if_t<is_enum_v<E>, int> = 0>
static constexpr details::names_generator_t<E, StringView, NullTerminated> names_generator{};

template<typename E, typename Pair = std::pair<E, string_view>, bool NullTerminated = true, std::enable_if_t<is_enum_v<E>, int> = 0>
static constexpr details::entries_generator_t<E, Pair, NullTerminated> entries_generator{};

#endif

} // namespace enchantum

#if defined(ENCAHNTUM_DETAILS_GCC_MAJOR) && ENCAHNTUM_DETAILS_GCC_MAJOR <= 10
  #pragma GCC diagnostic pop
#endif
