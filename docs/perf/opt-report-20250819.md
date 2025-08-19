# Optimization Evaluation Report (20250819)

## Environment
- OS: Linux
- Python: 3.12.3

## Build Results
### Baseline
- clean: wall=24.71s, max_rss=221640.0 KB
- incr: wall=0.81s, max_rss=17404.0 KB
- incr_after_touch: wall=0.8s, max_rss=129792.0 KB

### Optimized
- clean: wall=13.72s, max_rss=201948.0 KB
- incr: wall=0.92s, max_rss=21032.0 KB
- incr_after_touch: wall=3.89s, max_rss=111424.0 KB

## Runtime Benchmarks
- Geometric mean speedup (optimized vs baseline): 1.083x

| Benchmark | Baseline (ns) | Optimized (ns) | Speedup |
|---|---:|---:|---:|
| cast | 1 | 0 | 1.295x |
| to_string | 0 | 0 | 0.973x |
| values | 1 | 1 | 1.009x |