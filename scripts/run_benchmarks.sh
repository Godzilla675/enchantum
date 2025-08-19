#!/usr/bin/env bash
set -euo pipefail

# Usage: scripts/run_benchmarks.sh <preset>
# Discovers benchmark executables and runs them. Prefers Google Benchmark JSON.
PRESET="${1:-baseline}"
BUILD_DIR="$(jq -r '.configurePresets[] | select(.name=="'"$PRESET"'") | .binaryDir' CMakeUserPresets.json 2>/dev/null || true)"
if [[ -z "$BUILD_DIR" || "$BUILD_DIR" == "null" ]]; then
  echo "Could not resolve build dir for preset $PRESET"; exit 2
fi

OUT_DIR="docs/perf/raw/$(date +%Y%m%d)/$PRESET/bench"
mkdir -p "$OUT_DIR"

# Heuristic: benchmark binaries often live under $BUILD_DIR/benchmarks or have 'bench' in name.
mapfile -t CANDIDATES < <(find "$BUILD_DIR" -type f -perm -u+x \( -path "*/benchmarks/*" -o -iname "*bench*" -o -iname "*benchmark*" \) 2>/dev/null)

if [[ ${#CANDIDATES[@]} -eq 0 ]]; then
  echo "No benchmark executables found under $BUILD_DIR"; exit 0
fi

for exe in "${CANDIDATES[@]}"; do
  base="$(basename "$exe")"
  if "$exe" --help 2>/dev/null | grep -qi "benchmark_format"; then
    "$exe" --benchmark_format=json --benchmark_repetitions=10 > "$OUT_DIR/${base}.json" || true
  else
    /usr/bin/time -v "$exe" > "$OUT_DIR/${base}.txt" 2> "$OUT_DIR/${base}.time" || true
  fi
  echo "Ran $exe"
done

echo "Benchmark outputs saved to $OUT_DIR"