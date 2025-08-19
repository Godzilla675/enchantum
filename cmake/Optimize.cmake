# Cross-platform optimization and build-speed helpers.

include(CheckIPOSupported)

option(ENCH_USE_UNITY "Enable Unity (Jumbo) builds for faster compiles" ON)
option(ENCH_USE_PCH "Enable precompiled headers" ON)
option(ENCH_USE_LTO "Enable Link-Time Optimization/IPO in Release" ON)
option(ENCH_USE_CCACHE "Use ccache/sccache if available" ON)
option(ENCH_FAST_MATH "Enable aggressive/fast math in Release (use with care)" OFF)
option(ENCH_NATIVE_ARCH "Use -march=native (local builds only)" OFF)

# 1) Compiler launcher (ccache/sccache)
if(ENCH_USE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    message(STATUS "Using ccache")
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "" FORCE)
  else()
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
      message(STATUS "Using sccache")
      set(CMAKE_C_COMPILER_LAUNCHER   "${SCCACHE_PROGRAM}" CACHE STRING "" FORCE)
      set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}" CACHE STRING "" FORCE)
    endif()
  endif()
endif()

# 2) Unity builds
if(ENCH_USE_UNITY)
  set(CMAKE_UNITY_BUILD ON)
endif()

# 3) Global compile options per compiler
if(MSVC)
  add_compile_options(
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Release>:/DNDEBUG>
    $<$<CONFIG:Release>:/GL>
    $<$<CONFIG:Release>:/Gw>
    $<$<CONFIG:Release>:/Gy>
    $<$<CONFIG:RelWithDebInfo>:/O2>
    $<$<CONFIG:RelWithDebInfo>:/Zi>
    $<$<CONFIG:RelWithDebInfo>:/DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:/Gw>
    $<$<CONFIG:RelWithDebInfo>:/Gy>
  )
  add_link_options(
    $<$<CONFIG:Release>:/LTCG>
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
    $<$<CONFIG:RelWithDebInfo>:/LTCG>
    $<$<CONFIG:RelWithDebInfo>:/OPT:REF>
    $<$<CONFIG:RelWithDebInfo>:/OPT:ICF>
  )
else()
  add_compile_options(
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O3>
    $<$<CONFIG:RelWithDebInfo>:-g>
    $<$<CONFIG:RelWithDebInfo>:-DNDEBUG>
  )
  if(ENCH_FAST_MATH)
    add_compile_options(
      $<$<CONFIG:Release>:-ffast-math>
      $<$<CONFIG:Release>:-fno-math-errno>
    )
  endif()
  if(ENCH_NATIVE_ARCH)
    add_compile_options(
      $<$<CONFIG:Release>:-march=native>
      $<$<CONFIG:Release>:-mtune=native>
    )
  endif()
  find_program(LLD_LINKER NAMES ld.lld lld)
  if(LLD_LINKER)
    add_link_options(-fuse-ld=lld)
  endif()
endif()

# 4) IPO/LTO
if(ENCH_USE_LTO)
  check_ipo_supported(RESULT ipo_supported OUTPUT ipo_output)
  if(ipo_supported)
    message(STATUS "IPO/LTO is supported and enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
  else()
    message(STATUS "IPO/LTO not supported: ${ipo_output}")
  endif()
endif()

# 5) Convenience function to attach PCH and common warnings to a target.
function(enchantum_optimize_target target)
  if(ENCH_USE_PCH)
    target_precompile_headers(${target} PRIVATE
      <vector>
      <string>
      <string_view>
      <span>
      <memory>
      <cstdint>
      <utility>
      <algorithm>
    )
  endif()

  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive- /EHsc)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()