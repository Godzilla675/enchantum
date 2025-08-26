# Benchmark Results Summary - clang-18.1

## Speedup Analysis (Relative to NAIVE)

| Enum | Workload | Policy | Mean Speedup | P90 Speedup | Mean (ns) | P90 (ns) |
|------|----------|---------|--------------|-------------|-----------|----------|
| 13BucketHeavy16 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.71 |
| 13BucketHeavy16 | 100%_negative_diff_len | AUTO | 0.25x | 0.28x | 2.50 | 2.57 |
| 13BucketHeavy16 | 100%_negative_diff_len | LEN_FIRST | 0.25x | 0.28x | 2.51 | 2.56 |
| 13BucketHeavy16 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.62 |
| 13BucketHeavy16 | 100%_negative_same_len | AUTO | 0.25x | 0.24x | 2.51 | 2.59 |
| 13BucketHeavy16 | 100%_negative_same_len | LEN_FIRST | 0.25x | 0.24x | 2.51 | 2.57 |
| 13BucketHeavy16 | 100%_positive | NAIVE | 1.00x | 1.00x | 15.86 | 15.94 |
| 13BucketHeavy16 | 100%_positive | AUTO | 0.35x | 0.35x | 44.84 | 45.00 |
| 13BucketHeavy16 | 100%_positive | LEN_FIRST | 0.35x | 0.35x | 45.30 | 45.51 |
| 13BucketHeavy16 | 20%_positive | NAIVE | 1.00x | 1.00x | 3.67 | 3.77 |
| 13BucketHeavy16 | 20%_positive | AUTO | 0.31x | 0.29x | 11.67 | 12.97 |
| 13BucketHeavy16 | 20%_positive | LEN_FIRST | 0.31x | 0.32x | 11.81 | 11.96 |
| 13BucketHeavy16 | 80%_positive | NAIVE | 1.00x | 1.00x | 13.96 | 14.06 |
| 13BucketHeavy16 | 80%_positive | AUTO | 0.37x | 0.37x | 38.09 | 38.25 |
| 13BucketHeavy16 | 80%_positive | LEN_FIRST | 0.35x | 0.35x | 39.49 | 39.66 |
| 5Tiny3 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.65 |
| 5Tiny3 | 100%_negative_diff_len | AUTO | 0.89x | 0.86x | 0.71 | 0.76 |
| 5Tiny3 | 100%_negative_diff_len | LEN_FIRST | 1.02x | 0.97x | 0.62 | 0.67 |
| 5Tiny3 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.68 |
| 5Tiny3 | 100%_negative_same_len | AUTO | 1.03x | 0.99x | 0.61 | 0.69 |
| 5Tiny3 | 100%_negative_same_len | LEN_FIRST | 1.00x | 0.97x | 0.63 | 0.70 |
| 5Tiny3 | 100%_positive | NAIVE | 1.00x | 1.00x | 6.74 | 6.91 |
| 5Tiny3 | 100%_positive | AUTO | 6.81x | 6.40x | 0.99 | 1.08 |
| 5Tiny3 | 100%_positive | LEN_FIRST | 6.95x | 6.34x | 0.97 | 1.09 |
| 5Tiny3 | 20%_positive | NAIVE | 1.00x | 1.00x | 2.01 | 2.10 |
| 5Tiny3 | 20%_positive | AUTO | 2.91x | 2.66x | 0.69 | 0.79 |
| 5Tiny3 | 20%_positive | LEN_FIRST | 3.24x | 3.00x | 0.62 | 0.70 |
| 5Tiny3 | 80%_positive | NAIVE | 1.00x | 1.00x | 5.84 | 5.95 |
| 5Tiny3 | 80%_positive | AUTO | 6.71x | 6.33x | 0.87 | 0.94 |
| 5Tiny3 | 80%_positive | LEN_FIRST | 6.95x | 6.54x | 0.84 | 0.91 |
| 6Small8 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 1.56 | 1.63 |
| 6Small8 | 100%_negative_diff_len | AUTO | 0.99x | 0.99x | 1.57 | 1.64 |
| 6Small8 | 100%_negative_diff_len | LEN_FIRST | 0.62x | 0.63x | 2.50 | 2.57 |
| 6Small8 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 3.12 | 3.19 |
| 6Small8 | 100%_negative_same_len | AUTO | 1.00x | 0.99x | 3.13 | 3.21 |
| 6Small8 | 100%_negative_same_len | LEN_FIRST | 0.28x | 0.28x | 11.34 | 11.41 |
| 6Small8 | 100%_positive | NAIVE | 1.00x | 1.00x | 10.58 | 10.64 |
| 6Small8 | 100%_positive | AUTO | 0.99x | 0.99x | 10.73 | 10.80 |
| 6Small8 | 100%_positive | LEN_FIRST | 0.59x | 0.59x | 17.80 | 17.89 |
| 6Small8 | 20%_positive | NAIVE | 1.00x | 1.00x | 3.40 | 3.45 |
| 6Small8 | 20%_positive | AUTO | 1.00x | 0.99x | 3.41 | 3.48 |
| 6Small8 | 20%_positive | LEN_FIRST | 0.61x | 0.61x | 5.56 | 5.61 |
| 6Small8 | 80%_positive | NAIVE | 1.00x | 1.00x | 8.88 | 8.98 |
| 6Small8 | 80%_positive | AUTO | 0.99x | 0.99x | 8.99 | 9.04 |
| 6Small8 | 80%_positive | LEN_FIRST | 0.58x | 0.59x | 15.18 | 15.25 |
| 7Large16 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.50 | 0.59 |
| 7Large16 | 100%_negative_diff_len | AUTO | 0.20x | 0.23x | 2.53 | 2.58 |
| 7Large16 | 100%_negative_diff_len | LEN_FIRST | 0.20x | 0.23x | 2.51 | 2.58 |
| 7Large16 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.52 | 0.59 |
| 7Large16 | 100%_negative_same_len | AUTO | 0.21x | 0.23x | 2.51 | 2.57 |
| 7Large16 | 100%_negative_same_len | LEN_FIRST | 0.21x | 0.23x | 2.50 | 2.56 |
| 7Large16 | 100%_positive | NAIVE | 1.00x | 1.00x | 24.80 | 24.97 |
| 7Large16 | 100%_positive | AUTO | 1.18x | 1.18x | 21.00 | 21.09 |
| 7Large16 | 100%_positive | LEN_FIRST | 1.11x | 1.12x | 22.27 | 22.36 |
| 7Large16 | 20%_positive | NAIVE | 1.00x | 1.00x | 5.90 | 7.43 |
| 7Large16 | 20%_positive | AUTO | 0.95x | 1.19x | 6.18 | 6.23 |
| 7Large16 | 20%_positive | LEN_FIRST | 0.91x | 1.14x | 6.46 | 6.50 |
| 7Large16 | 80%_positive | NAIVE | 1.00x | 1.00x | 21.47 | 21.57 |
| 7Large16 | 80%_positive | AUTO | 1.15x | 1.15x | 18.60 | 18.68 |
| 7Large16 | 80%_positive | LEN_FIRST | 1.08x | 1.08x | 19.92 | 20.00 |
| 7Large32 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.51 | 0.59 |
| 7Large32 | 100%_negative_diff_len | AUTO | 0.20x | 0.23x | 2.51 | 2.57 |
| 7Large32 | 100%_negative_diff_len | LEN_FIRST | 0.20x | 0.23x | 2.50 | 2.57 |
| 7Large32 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 50.77 | 51.47 |
| 7Large32 | 100%_negative_same_len | AUTO | 4.31x | 4.31x | 11.79 | 11.93 |
| 7Large32 | 100%_negative_same_len | LEN_FIRST | 4.22x | 4.26x | 12.02 | 12.09 |
| 7Large32 | 100%_positive | NAIVE | 1.00x | 1.00x | 39.00 | 39.02 |
| 7Large32 | 100%_positive | AUTO | 1.40x | 1.39x | 27.91 | 28.04 |
| 7Large32 | 100%_positive | LEN_FIRST | 1.44x | 1.43x | 27.16 | 27.24 |
| 7Large32 | 20%_positive | NAIVE | 1.00x | 1.00x | 29.50 | 29.57 |
| 7Large32 | 20%_positive | AUTO | 3.17x | 3.17x | 9.30 | 9.34 |
| 7Large32 | 20%_positive | LEN_FIRST | 3.35x | 3.33x | 8.81 | 8.89 |
| 7Large32 | 80%_positive | NAIVE | 1.00x | 1.00x | 38.54 | 38.66 |
| 7Large32 | 80%_positive | AUTO | 1.54x | 1.54x | 24.95 | 25.05 |
| 7Large32 | 80%_positive | LEN_FIRST | 1.59x | 1.59x | 24.29 | 24.35 |
| 7Large64 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.63 | 0.62 |
| 7Large64 | 100%_negative_diff_len | AUTO | 0.25x | 0.24x | 2.51 | 2.57 |
| 7Large64 | 100%_negative_diff_len | LEN_FIRST | 0.25x | 0.24x | 2.51 | 2.60 |
| 7Large64 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 0.73 | 0.81 |
| 7Large64 | 100%_negative_same_len | AUTO | 0.29x | 0.32x | 2.51 | 2.57 |
| 7Large64 | 100%_negative_same_len | LEN_FIRST | 0.29x | 0.32x | 2.51 | 2.57 |
| 7Large64 | 100%_positive | NAIVE | 1.00x | 1.00x | 57.55 | 57.76 |
| 7Large64 | 100%_positive | AUTO | 1.54x | 1.55x | 37.25 | 37.36 |
| 7Large64 | 100%_positive | LEN_FIRST | 1.57x | 1.56x | 36.72 | 36.93 |
| 7Large64 | 20%_positive | NAIVE | 1.00x | 1.00x | 12.18 | 12.26 |
| 7Large64 | 20%_positive | AUTO | 1.26x | 1.22x | 9.69 | 10.03 |
| 7Large64 | 20%_positive | LEN_FIRST | 1.31x | 1.30x | 9.32 | 9.42 |
| 7Large64 | 80%_positive | NAIVE | 1.00x | 1.00x | 48.62 | 48.85 |
| 7Large64 | 80%_positive | AUTO | 1.52x | 1.52x | 31.98 | 32.13 |
| 7Large64 | 80%_positive | LEN_FIRST | 1.53x | 1.53x | 31.82 | 31.95 |
| 8Medium12 | 100%_negative_diff_len | NAIVE | 1.00x | 1.00x | 0.57 | 0.65 |
| 8Medium12 | 100%_negative_diff_len | AUTO | 0.85x | 0.87x | 0.67 | 0.75 |
| 8Medium12 | 100%_negative_diff_len | LEN_FIRST | 0.23x | 0.25x | 2.50 | 2.56 |
| 8Medium12 | 100%_negative_same_len | NAIVE | 1.00x | 1.00x | 12.01 | 12.07 |
| 8Medium12 | 100%_negative_same_len | AUTO | 1.00x | 1.00x | 12.00 | 12.07 |
| 8Medium12 | 100%_negative_same_len | LEN_FIRST | 2.72x | 2.65x | 4.41 | 4.56 |
| 8Medium12 | 100%_positive | NAIVE | 1.00x | 1.00x | 24.74 | 24.85 |
| 8Medium12 | 100%_positive | AUTO | 0.99x | 0.99x | 24.95 | 25.05 |
| 8Medium12 | 100%_positive | LEN_FIRST | 1.10x | 1.10x | 22.39 | 22.55 |
| 8Medium12 | 20%_positive | NAIVE | 1.00x | 1.00x | 5.21 | 5.30 |
| 8Medium12 | 20%_positive | AUTO | 0.95x | 0.96x | 5.46 | 5.53 |
| 8Medium12 | 20%_positive | LEN_FIRST | 0.81x | 0.81x | 6.44 | 6.56 |
| 8Medium12 | 80%_positive | NAIVE | 1.00x | 1.00x | 21.39 | 21.59 |
| 8Medium12 | 80%_positive | AUTO | 1.00x | 1.00x | 21.39 | 21.49 |
| 8Medium12 | 80%_positive | LEN_FIRST | 1.08x | 1.08x | 19.84 | 19.93 |

## Performance Summary

**Average Speedups (vs NAIVE):**

- AUTO: 1.40x (mean), 1.37x (p90)
- LEN_FIRST: 1.39x (mean), 1.36x (p90)
