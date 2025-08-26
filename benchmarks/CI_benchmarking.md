# CI Benchmarking System

This document describes the automated benchmarking system for the Enchantum library, which compares the performance of different enum dispatch policies across multiple compilers.

## Overview

The benchmarking system automatically:

1. **Builds** separate executables for each policy (AUTO, NAIVE, LEN_FIRST) to avoid main() symbol conflicts
2. **Runs** benchmarks across multiple enum sizes and workload patterns
3. **Collects** CSV performance data from each policy
4. **Analyzes** speedup metrics relative to the NAIVE baseline
5. **Generates** summary reports with performance comparisons

## Dispatch Policies

- **NAIVE**: Sequential string comparison (baseline for speedup calculations)
- **AUTO**: Automatically selected optimization strategy  
- **LEN_FIRST**: Length-based filtering before string comparison

## Benchmark Workloads

Each enum type is tested with the following workload patterns:

- `100%_positive`: All lookups find valid enum values
- `80%_positive`: 80% valid lookups, 20% invalid
- `20%_positive`: 20% valid lookups, 80% invalid  
- `100%_negative_diff_len`: All invalid lookups with different string lengths
- `100%_negative_same_len`: All invalid lookups with same string length as valid enums

## Enum Test Cases

- **Tiny3**: 3 values (A, B, C)
- **Small8**: 8 fruit names
- **Medium12**: 12 Greek letters  
- **Large16**: 16 programming languages
- **Large32**: 32 technology terms
- **Large64**: 64 numbered technology terms
- **BucketHeavy16**: 16 values designed to stress hash bucket collisions

## Running Benchmarks Locally

### Prerequisites

- CMake 3.22+
- C++17 compatible compiler
- Python 3.x

### Build and Run

```bash
# Configure with benchmarks enabled
mkdir build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENCHANTUM_BUILD_BENCHMARKS=ON

# Build benchmark executables
cmake --build build

# Run orchestration script
python3 scripts/run_benchmarks.py --build-dir build/benchmarks
```

### Environment Variables

- `ENCHANTUM_BENCH_RUNS`: Number of timing runs per test (default: 50)
- `ENCHANTUM_BENCH_WARMUP`: Number of warmup iterations (default: 10)

## Output Artifacts

For each compiler (e.g., `gcc-13.3`, `clang-18`, `msvc`):

### Raw CSV Files
- `raw_csv/{compiler}_AUTO.csv`
- `raw_csv/{compiler}_NAIVE.csv` 
- `raw_csv/{compiler}_LEN_FIRST.csv`

### Combined Data
- `combined_{compiler}.csv`: All policies merged with policy column
- `summary_{compiler}.md`: Markdown table with speedup analysis

## Understanding Results

### Speedup Calculation
```
Speedup = NAIVE_time / Policy_time
```

- **> 1.0x**: Policy is faster than NAIVE
- **< 1.0x**: Policy is slower than NAIVE  
- **1.0x**: Same performance as NAIVE

### Metrics Reported
- **Mean Speedup**: Average performance across 50 runs
- **P90 Speedup**: 90th percentile performance (less affected by outliers)
- **Mean (ns)**: Average time per lookup in nanoseconds
- **P90 (ns)**: 90th percentile time per lookup

## CI Integration

The GitHub Actions workflow (`.github/workflows/bench.yml`) runs benchmarks on:

- **Linux**: GCC 13, Clang 18
- **Windows**: MSVC 2022

Results are uploaded as build artifacts and displayed in the workflow summary.

## Extending the System

### Adding New Policies

1. **Add executable target** in `benchmarks/CMakeLists.txt`:
   ```cmake
   add_executable(enchantum_bench_newpolicy policy_bench_newpolicy.cpp)
   # ... configure target ...
   target_compile_definitions(enchantum_bench_newpolicy PRIVATE ENCHANTUM_ENUM_DISPATCH_POLICY=3)
   ```

2. **Update script mapping** in `scripts/run_benchmarks.py`:
   ```python
   self.policies = {
       "auto": "AUTO",
       "naive": "NAIVE", 
       "len_first": "LEN_FIRST",
       "newpolicy": "NEWPOLICY"  # Add this
   }
   ```

3. **Create benchmark source** following the pattern of existing `policy_bench_*.cpp` files

### Adding New Enum Types

Add new enum definitions and corresponding `get_valid_names<>()` specializations to the benchmark source files.

## Performance Considerations

- **Noise Mitigation**: Uses 50 runs + 10 warmup iterations by default
- **GitHub Runners**: Performance numbers will vary on shared CI infrastructure
- **Compiler Optimizations**: Release builds with `-O2`/`/O2` optimization
- **Measurement Precision**: Nanosecond timing resolution via `std::chrono::steady_clock`

## Troubleshooting

### Build Issues
- Ensure `ENCHANTUM_BUILD_BENCHMARKS=ON` is set
- Check that all benchmark source files include `compiler_id.hpp`
- Verify CMake targets don't conflict (each policy needs separate executable)

### Script Issues  
- Check executable permissions on `scripts/run_benchmarks.py`
- Ensure Python 3.x is available
- Verify benchmark executables exist in expected build directory

### Performance Anomalies
- Check for thermal throttling on extended runs
- Consider increasing run count for more stable averages
- Compare relative trends rather than absolute numbers across different systems