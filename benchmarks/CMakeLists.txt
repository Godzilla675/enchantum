include(CheckCXXCompilerFlag)

include(FetchContent)
FetchContent_Declare(magic_enum
	GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
	GIT_SHALLOW ON
	GIT_TAG master)
FetchContent_MakeAvailable(magic_enum)

# Define common compiler options
if(MSVC)
  set(BENCHMARK_COMPILE_OPTIONS /O2 /permissive- /W4)
else()
  set(BENCHMARK_COMPILE_OPTIONS -O2 -Wall -Wextra -Wconversion -Wpedantic)
endif()

# Create separate executables for each policy to avoid main() conflicts
add_executable(enchantum_bench_auto policy_bench_auto.cpp)
target_include_directories(enchantum_bench_auto PRIVATE "${magic_enum_SOURCE_DIR}/include")
target_link_libraries(enchantum_bench_auto enchantum::enchantum)
target_compile_options(enchantum_bench_auto PRIVATE ${BENCHMARK_COMPILE_OPTIONS})
target_compile_definitions(enchantum_bench_auto PRIVATE ENCHANTUM_ENUM_DISPATCH_POLICY=0)

add_executable(enchantum_bench_naive policy_bench_naive.cpp)
target_include_directories(enchantum_bench_naive PRIVATE "${magic_enum_SOURCE_DIR}/include")
target_link_libraries(enchantum_bench_naive enchantum::enchantum)
target_compile_options(enchantum_bench_naive PRIVATE ${BENCHMARK_COMPILE_OPTIONS})
target_compile_definitions(enchantum_bench_naive PRIVATE ENCHANTUM_ENUM_DISPATCH_POLICY=1)

add_executable(enchantum_bench_len_first policy_bench_len_first.cpp)
target_include_directories(enchantum_bench_len_first PRIVATE "${magic_enum_SOURCE_DIR}/include")
target_link_libraries(enchantum_bench_len_first enchantum::enchantum)
target_compile_options(enchantum_bench_len_first PRIVATE ${BENCHMARK_COMPILE_OPTIONS})
target_compile_definitions(enchantum_bench_len_first PRIVATE ENCHANTUM_ENUM_DISPATCH_POLICY=2)
