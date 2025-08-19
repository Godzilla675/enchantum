# Performance Reports

- Raw logs live under `docs/perf/raw/YYYYMMDD/`.
- Summary reports are `docs/perf/opt-report-YYYYMMDD.md`.

To reproduce locally:
1. Baseline
   - `cmake --preset=baseline`
   - `cmake --build --preset=baseline`
   - `bash scripts/measure_build.sh baseline clean`
   - `bash scripts/measure_build.sh baseline incr <some_source.cpp>`
   - `bash scripts/run_benchmarks.sh baseline`

2. Optimized
   - `cmake --preset=optimized`
   - `cmake --build --preset=optimized`
   - `bash scripts/measure_build.sh optimized clean`
   - `bash scripts/measure_build.sh optimized incr <some_source.cpp>`
   - `bash scripts/run_benchmarks.sh optimized`

3. Summarize
   - `python3 scripts/summarize.py`