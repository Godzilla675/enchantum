# Compile Time Optimization Guide

This document describes the compile-time optimizations available in enchantum and how to use them for faster builds.

## Optimization Overview

Enchantum provides several entry points for different use cases, allowing you to include only the functionality you need:

### Entry Point Headers

1. **`enchantum/core.hpp`** - Lightweight core functionality
   - Includes: `to_string`, `cast`, `contains`, basic metadata
   - Excludes: iostream operators, formatting, containers, generators
   - Best for: Core enum reflection only
   - Compile time: ~0.78s (fastest)

2. **`enchantum/enchantum.hpp`** - Direct core functionality
   - Includes: Core reflection functions
   - Best for: When you only need basic enum operations
   - Compile time: ~0.79s

3. **`enchantum/all.hpp`** - Optimized complete functionality  
   - Includes: All features but conditionally compiled
   - Requires explicit flags for optional features
   - Best for: When you want everything but optimized
   - Compile time: ~0.83s

4. **`enchantum/all_features.hpp`** - Full backward compatibility
   - Includes: All features unconditionally
   - Best for: Drop-in replacement for existing code
   - Compile time: ~0.83s

## Conditional Compilation Flags

You can control which features are included using these preprocessor definitions:

```cpp
#define ENCHANTUM_ENABLE_IOSTREAM    // Enable iostream operators
#define ENCHANTUM_ENABLE_FORMATTING  // Enable std::format/fmt support
#define ENCHANTUM_NO_EXCEPTIONS      // Disable exception support
```

## Usage Examples

### For maximum compile speed (core functionality only):
```cpp
#include <enchantum/core.hpp>

enum class Color { Red, Green, Blue };

int main() {
    auto name = enchantum::to_string(Color::Red);        // "Red"
    auto color = enchantum::cast<Color>("Blue");         // Color::Blue
    bool valid = enchantum::contains<Color>("Green");    // true
}
```

### For iostream support:
```cpp
#define ENCHANTUM_ENABLE_IOSTREAM
#include <enchantum/all.hpp>

enum class Status { Active, Inactive };

int main() {
    Status s = Status::Active;
    std::cout << s << std::endl;  // Prints: Active
}
```

### For formatting support:
```cpp
#define ENCHANTUM_ENABLE_FORMATTING
#include <enchantum/all.hpp>
#include <format>

enum class Level { Info, Warning, Error };

int main() {
    auto msg = std::format("Log level: {}", Level::Warning);
}
```

### For full compatibility (all features):
```cpp
#include <enchantum/all_features.hpp>
// Everything is available
```

## CMake Options

When using enchantum with CMake, you can control default behavior:

```cmake
target_link_libraries(your_target enchantum::enchantum)

# Optional: Enable features by default
target_compile_definitions(your_target PRIVATE 
    ENCHANTUM_ENABLE_IOSTREAM=1
    ENCHANTUM_ENABLE_FORMATTING=1
)

# Optional: Enable precompiled headers for faster builds
set(ENCHANTUM_USE_PCH ON)
```

## Precompiled Headers

For projects that heavily use enchantum, you can enable precompiled headers:

```cpp
// In your precompiled header file
#include <enchantum/pch.hpp>
```

Or via CMake:
```cmake
set(ENCHANTUM_USE_PCH ON CACHE BOOL "Use precompiled headers")
```

## Performance Comparison

| Include Pattern | Preprocessed Lines | Avg Compile Time | Features |
|---|---|---|---|
| `core.hpp` | 83,920 | 0.797s | Core reflection only |
| `enchantum.hpp` | 83,915 | 0.786s | Core functions |
| `all.hpp` (optimized) | 85,815 | 0.826s | Conditional features |
| `all_features.hpp` | 85,950 | 0.827s | All features |

## Migration Guide

### From magic_enum
Replace:
```cpp
#include <magic_enum.hpp>
auto name = magic_enum::enum_name(value);
```

With (optimized):
```cpp
#include <enchantum/core.hpp>
auto name = enchantum::to_string(value);
```

### From previous enchantum versions
For backward compatibility, simply replace:
```cpp
#include <enchantum/all.hpp>
```

With:
```cpp
#include <enchantum/all_features.hpp>
```

Or optimize by using:
```cpp
#define ENCHANTUM_ENABLE_IOSTREAM
#define ENCHANTUM_ENABLE_FORMATTING
#include <enchantum/all.hpp>
```