# C++14 Support Status

## Summary

**Enchantum requires C++17 as a minimum**. While partial C++14 compatibility infrastructure has been added, full C++14 support is not feasible without major architectural changes that would negatively impact compile times and code maintainability.

## Why C++17 is Required

Enchantum's compile-time enum reflection relies on several C++17 features that have no direct C++14 equivalent:

### 1. `auto` Template Parameters (Critical Blocker)

The core reflection mechanism uses non-type template parameters with `auto`:

```cpp
template<auto Enum>  // C++17 feature - NOT available in C++14
constexpr auto get_enum_name() noexcept { ... }
```

**C++14 limitation**: Non-type template parameters must have their type explicitly specified. There's no way to accept any enum value generically without knowing its type upfront.

**Impact**: This feature is used in 12+ places across compiler-specific implementation files and is fundamental to how the library performs reflection.

**C++14 workaround**: Would require complete redesign of the reflection mechanism, likely using macros or code generation.

### 2. `if constexpr` (45 occurrences)

Used extensively for compile-time branching:

```cpp
if constexpr (std::is_signed_v<T>) {
  // signed path
} else {
  // unsigned path  
}
```

**C++14 limitation**: Regular `if` with dependent types requires both branches to compile, even if one is never taken. This causes compilation failures in many cases.

**C++14 workaround**: Use tag dispatch, SFINAE, or template specialization - significantly more verbose and harder to maintain.

### 3. Inline Variables

Variable templates use `inline` for proper linkage:

```cpp
template<typename E>
inline constexpr auto values = details::get_values<E>();
```

**C++14 limitation**: Variable templates exist but `inline` keyword for variables doesn't. Without `inline`, ODR violations can occur in header-only libraries.

**C++14 workaround**: Use namespace-level `static constexpr` (causes code bloat) or constexpr functions (different syntax).

## What Has Been Added

The following C++14 compatibility infrastructure has been added to make the codebase more portable:

1. **Type trait compatibility layer** (`common.hpp`):
   - Provides `_v` shortcuts (`is_enum_v`, `is_same_v`, etc.) for C++14
   - Adds `ENCHANTUM_INLINE_VAR` macro for inline variable compatibility

2. **string_view polyfill** (`details/string_view.hpp`):
   - Minimal `string_view` implementation for C++14
   - Automatically uses `std::string_view` in C++17+

3. **Conditional compilation**:
   - CMakeLists.txt updated to respect `CMAKE_CXX_STANDARD`
   - Proper C++20 guards for `std::format` support

## Attempting C++14 Compilation

If you try to compile with C++14, you'll encounter errors like:

```
error: 'auto' parameter not permitted in this context
```

This is expected and cannot be worked around without fundamental changes to the library.

## Recommendations

1. **Use C++17 or later** - This is the supported configuration and provides the best compile times and user experience.

2. **If C++14 is absolutely required**:
   - Consider using an alternative enum reflection library that targets C++14
   - Or use macro-based solutions like BETTER_ENUMS
   - Or wait for the library to potentially support C++14 in a future major version (would require significant refactoring)

3. **For best performance**: Use C++20 or later to benefit from concepts and improved compile times.

## Testing

The library is tested with:
- C++17 ✓ (minimum supported)
- C++20 ✓
- C++23 ✓ (where available)

C++14 is intentionally not tested as it's not supported.
