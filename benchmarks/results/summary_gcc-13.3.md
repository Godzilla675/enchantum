# Benchmark Results Summary - gcc-13.3

## Speedup Analysis (Relative to NAIVE)

| Enum | Workload | Policy | Mean Speedup | P90 Speedup | Mean (ns) | P90 (ns) |
|------|----------|---------|--------------|-------------|-----------|----------|
| 13BucketHeavy16 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.66 |
| 13BucketHeavy16 | 100%_negative_diff_len | AUTO | 0.06x | 0.07x | 9.99 | 10.03 |
| 13BucketHeavy16 | 100%_negative_diff_len | LEN_FIRST | 0.06x | 0.07x | 9.98 | 10.02 |
| 13BucketHeavy16 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.65 | 0.71 |
| 13BucketHeavy16 | 100%_negative_same_len | AUTO | 0.07x | 0.07x | 10.00 | 10.06 |
| 13BucketHeavy16 | 100%_negative_same_len | LEN_FIRST | 0.07x | 0.07x | 9.99 | 10.02 |
| 13BucketHeavy16 | 100%_positive | NAIVE | 1.00x | 1.00x | 19.13 | 19.26 |
| 13BucketHeavy16 | 100%_positive | AUTO | 0.41x | 0.40x | 47.15 | 47.60 |
| 13BucketHeavy16 | 100%_positive | LEN_FIRST | 0.42x | 0.42x | 45.70 | 45.84 |
| 13BucketHeavy16 | 20%_positive | NAIVE | 1.00x | 1.00x | 4.42 | 4.49 |
| 13BucketHeavy16 | 20%_positive | AUTO | 0.32x | 0.32x | 14.01 | 14.16 |
| 13BucketHeavy16 | 20%_positive | LEN_FIRST | 0.33x | 0.33x | 13.36 | 13.42 |
| 13BucketHeavy16 | 80%_positive | NAIVE | 1.00x | 1.00x | 16.67 | 16.78 |
| 13BucketHeavy16 | 80%_positive | AUTO | 0.41x | 0.41x | 40.66 | 40.74 |
| 13BucketHeavy16 | 80%_positive | LEN_FIRST | 0.43x | 0.43x | 39.09 | 39.15 |
| 5Tiny3 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.65 | 0.71 |
| 5Tiny3 | 100%_negative_diff_len | AUTO | 0.98x | 0.96x | 0.66 | 0.74 |
| 5Tiny3 | 100%_negative_diff_len | LEN_FIRST | 1.00x | 1.00x | 0.65 | 0.71 |
| 5Tiny3 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.67 |
| 5Tiny3 | 100%_negative_same_len | AUTO | 0.88x | 0.80x | 0.72 | 0.84 |
| 5Tiny3 | 100%_negative_same_len | LEN_FIRST | 0.89x | 0.76x | 0.71 | 0.88 |
| 5Tiny3 | 100%_positive | NAIVE | 1.00x | 1.00x | 8.00 | 8.12 |
| 5Tiny3 | 100%_positive | AUTO | 1.24x | 1.24x | 6.46 | 6.56 |
| 5Tiny3 | 100%_positive | LEN_FIRST | 1.22x | 1.21x | 6.58 | 6.72 |
| 5Tiny3 | 20%_positive | NAIVE | 1.00x | 1.00x | 2.05 | 2.12 |
| 5Tiny3 | 20%_positive | AUTO | 1.14x | 1.13x | 1.80 | 1.87 |
| 5Tiny3 | 20%_positive | LEN_FIRST | 1.08x | 1.07x | 1.90 | 1.99 |
| 5Tiny3 | 80%_positive | NAIVE | 1.00x | 1.00x | 6.62 | 6.71 |
| 5Tiny3 | 80%_positive | AUTO | 1.24x | 1.23x | 5.36 | 5.44 |
| 5Tiny3 | 80%_positive | LEN_FIRST | 1.21x | 1.20x | 5.48 | 5.59 |
| 6Small8 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.64 | 0.70 |
| 6Small8 | 100%_negative_diff_len | AUTO | 0.98x | 0.99x | 0.65 | 0.71 |
| 6Small8 | 100%_negative_diff_len | LEN_FIRST | 0.06x | 0.07x | 9.98 | 10.02 |
| 6Small8 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 13.39 | 13.59 |
| 6Small8 | 100%_negative_same_len | AUTO | 1.06x | 1.03x | 12.68 | 13.20 |
| 6Small8 | 100%_negative_same_len | LEN_FIRST | 1.04x | 1.05x | 12.85 | 12.94 |
| 6Small8 | 100%_positive | NAIVE | 1.00x | 1.00x | 18.47 | 18.55 |
| 6Small8 | 100%_positive | AUTO | 1.01x | 1.01x | 18.21 | 18.34 |
| 6Small8 | 100%_positive | LEN_FIRST | 1.03x | 1.03x | 17.89 | 17.98 |
| 6Small8 | 20%_positive | NAIVE | 1.00x | 1.00x | 4.14 | 4.30 |
| 6Small8 | 20%_positive | AUTO | 0.99x | 1.01x | 4.17 | 4.24 |
| 6Small8 | 20%_positive | LEN_FIRST | 0.39x | 0.41x | 10.54 | 10.61 |
| 6Small8 | 80%_positive | NAIVE | 1.00x | 1.00x | 16.09 | 16.20 |
| 6Small8 | 80%_positive | AUTO | 1.02x | 1.01x | 15.84 | 16.00 |
| 6Small8 | 80%_positive | LEN_FIRST | 1.04x | 1.04x | 15.54 | 15.64 |
| 7Large16 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.64 | 0.71 |
| 7Large16 | 100%_negative_diff_len | AUTO | 0.06x | 0.07x | 10.00 | 10.07 |
| 7Large16 | 100%_negative_diff_len | LEN_FIRST | 0.06x | 0.07x | 9.98 | 10.01 |
| 7Large16 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.65 | 0.71 |
| 7Large16 | 100%_negative_same_len | AUTO | 0.07x | 0.07x | 9.99 | 10.05 |
| 7Large16 | 100%_negative_same_len | LEN_FIRST | 0.07x | 0.07x | 9.99 | 10.03 |
| 7Large16 | 100%_positive | NAIVE | 1.00x | 1.00x | 26.27 | 26.76 |
| 7Large16 | 100%_positive | AUTO | 1.22x | 1.24x | 21.50 | 21.63 |
| 7Large16 | 100%_positive | LEN_FIRST | 1.23x | 1.25x | 21.28 | 21.40 |
| 7Large16 | 20%_positive | NAIVE | 1.00x | 1.00x | 5.82 | 5.89 |
| 7Large16 | 20%_positive | AUTO | 0.55x | 0.55x | 10.59 | 10.64 |
| 7Large16 | 20%_positive | LEN_FIRST | 0.55x | 0.55x | 10.58 | 10.65 |
| 7Large16 | 80%_positive | NAIVE | 1.00x | 1.00x | 22.90 | 23.07 |
| 7Large16 | 80%_positive | AUTO | 1.20x | 1.20x | 19.03 | 19.15 |
| 7Large16 | 80%_positive | LEN_FIRST | 1.20x | 1.20x | 19.12 | 19.20 |
| 7Large32 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.65 | 0.77 |
| 7Large32 | 100%_negative_diff_len | AUTO | 0.07x | 0.08x | 9.99 | 10.05 |
| 7Large32 | 100%_negative_diff_len | LEN_FIRST | 0.07x | 0.08x | 9.97 | 10.00 |
| 7Large32 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 43.94 | 45.01 |
| 7Large32 | 100%_negative_same_len | AUTO | 3.11x | 3.16x | 14.15 | 14.25 |
| 7Large32 | 100%_negative_same_len | LEN_FIRST | 3.11x | 3.17x | 14.14 | 14.22 |
| 7Large32 | 100%_positive | NAIVE | 1.00x | 1.00x | 38.88 | 39.49 |
| 7Large32 | 100%_positive | AUTO | 1.47x | 1.49x | 26.38 | 26.45 |
| 7Large32 | 100%_positive | LEN_FIRST | 1.35x | 1.36x | 28.78 | 28.94 |
| 7Large32 | 20%_positive | NAIVE | 1.00x | 1.00x | 28.29 | 30.00 |
| 7Large32 | 20%_positive | AUTO | 2.54x | 2.68x | 11.12 | 11.18 |
| 7Large32 | 20%_positive | LEN_FIRST | 2.53x | 2.66x | 11.20 | 11.27 |
| 7Large32 | 80%_positive | NAIVE | 1.00x | 1.00x | 37.37 | 37.95 |
| 7Large32 | 80%_positive | AUTO | 1.58x | 1.59x | 23.70 | 23.83 |
| 7Large32 | 80%_positive | LEN_FIRST | 1.45x | 1.47x | 25.77 | 25.90 |
| 7Large64 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.66 |
| 7Large64 | 100%_negative_diff_len | AUTO | 0.06x | 0.07x | 9.99 | 10.06 |
| 7Large64 | 100%_negative_diff_len | LEN_FIRST | 0.06x | 0.07x | 9.98 | 10.00 |
| 7Large64 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.66 | 0.72 |
| 7Large64 | 100%_negative_same_len | AUTO | 0.07x | 0.07x | 9.98 | 10.01 |
| 7Large64 | 100%_negative_same_len | LEN_FIRST | 0.07x | 0.07x | 9.97 | 9.99 |
| 7Large64 | 100%_positive | NAIVE | 1.00x | 1.00x | 62.84 | 63.26 |
| 7Large64 | 100%_positive | AUTO | 1.61x | 1.61x | 38.93 | 39.29 |
| 7Large64 | 100%_positive | LEN_FIRST | 1.62x | 1.63x | 38.72 | 38.82 |
| 7Large64 | 20%_positive | NAIVE | 1.00x | 1.00x | 12.94 | 13.15 |
| 7Large64 | 20%_positive | AUTO | 1.11x | 1.12x | 11.65 | 11.72 |
| 7Large64 | 20%_positive | LEN_FIRST | 1.15x | 1.15x | 11.28 | 11.39 |
| 7Large64 | 80%_positive | NAIVE | 1.00x | 1.00x | 51.81 | 52.05 |
| 7Large64 | 80%_positive | AUTO | 1.55x | 1.54x | 33.51 | 33.71 |
| 7Large64 | 80%_positive | LEN_FIRST | 1.55x | 1.55x | 33.53 | 33.68 |
| 8Medium12 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.64 | 0.71 |
| 8Medium12 | 100%_negative_diff_len | AUTO | 0.93x | 0.80x | 0.69 | 0.89 |
| 8Medium12 | 100%_negative_diff_len | LEN_FIRST | 0.06x | 0.07x | 9.99 | 10.06 |
| 8Medium12 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 12.94 | 13.20 |
| 8Medium12 | 100%_negative_same_len | AUTO | 1.05x | 1.02x | 12.35 | 12.97 |
| 8Medium12 | 100%_negative_same_len | LEN_FIRST | 1.26x | 1.27x | 10.31 | 10.38 |
| 8Medium12 | 100%_positive | NAIVE | 1.00x | 1.00x | 26.38 | 26.46 |
| 8Medium12 | 100%_positive | AUTO | 1.04x | 1.04x | 25.33 | 25.45 |
| 8Medium12 | 100%_positive | LEN_FIRST | 1.11x | 1.10x | 23.68 | 24.12 |
| 8Medium12 | 20%_positive | NAIVE | 1.00x | 1.00x | 5.82 | 5.88 |
| 8Medium12 | 20%_positive | AUTO | 1.02x | 1.01x | 5.70 | 5.81 |
| 8Medium12 | 20%_positive | LEN_FIRST | 0.55x | 0.55x | 10.64 | 10.70 |
| 8Medium12 | 80%_positive | NAIVE | 1.00x | 1.00x | 23.15 | 23.25 |
| 8Medium12 | 80%_positive | AUTO | 1.05x | 1.04x | 22.14 | 22.27 |
| 8Medium12 | 80%_positive | LEN_FIRST | 1.07x | 1.07x | 21.65 | 21.79 |

## Performance Summary

**Average Speedups (vs NAIVE):**

- AUTO: 0.95x (mean), 0.95x (p90)
- LEN_FIRST: 0.87x (mean), 0.87x (p90)
