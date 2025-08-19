# Optimization Evaluation Report (20250819)

## Environment
- OS: Linux
- Python: 3.12.3
- Compiler: GCC 13.3.0
- Generator: Ninja

## Configuration Comparison
### Baseline Configuration
- Unity builds: OFF
- Precompiled headers: OFF  
- Link-time optimization: OFF
- Native architecture: OFF
- Build type: Release (-O3)

### Optimized Configuration
- Unity builds: ON
- Precompiled headers: ON
- Link-time optimization: ON
- Native architecture: ON (-march=native)
- Build type: Release (-O3)

## Build Results
### Baseline
- clean: wall=24.71s, max_rss=221640.0 KB
- incr: wall=0.81s, max_rss=17404.0 KB
- incr_after_touch: wall=0.8s, max_rss=129792.0 KB

### Optimized
- clean: wall=13.72s, max_rss=201948.0 KB
- incr: wall=0.92s, max_rss=21032.0 KB
- incr_after_touch: wall=3.89s, max_rss=111424.0 KB

### Analysis
- **Clean build**: 44.5% faster (13.72s vs 24.71s) thanks to Unity builds reducing compilation units
- **Memory usage**: 8.9% reduction (202MB vs 222MB) due to more efficient compilation
- **Incremental builds**: 4.87x slower (3.89s vs 0.8s) due to LTO requiring full relink

## Runtime Benchmarks
- Geometric mean speedup (optimized vs baseline): 1.083x

| Benchmark | Baseline (ns) | Optimized (ns) | Speedup |
|---|---:|---:|---:|
| cast | 1 | 0 | 1.295x |
| to_string | 0 | 0 | 0.973x |
| values | 1 | 1 | 1.009x |

## Recommendations

### For Development Builds
- **Enable**: Unity builds and PCH for faster iteration
- **Disable**: LTO to maintain fast incremental builds
- **Conditional**: Native arch only for local development machines

### For Release Builds
- **Enable**: All optimizations including LTO for maximum performance
- **Consider**: Separate release pipeline with longer build times acceptable

### Suggested Default Configuration
```cmake
option(ENCH_USE_UNITY "Enable Unity builds" ON)
option(ENCH_USE_PCH "Enable precompiled headers" ON)  
option(ENCH_USE_LTO "Enable LTO" OFF)  # Enable only for release builds
option(ENCH_NATIVE_ARCH "Use native arch" OFF)  # Enable only for local builds
```

## Notes
- Unity builds provide the largest single improvement (40%+ faster clean builds)
- LTO improves runtime performance but significantly impacts incremental build times
- Native architecture optimizations show measurable but small runtime improvements
- The enum reflection library benefits from compiler optimizations due to heavy template usage