#!/usr/bin/env bash
set -euo pipefail

# Usage: scripts/measure_build.sh <preset> <phase> [touch_file]
# phase: clean | incr
# Example: scripts/measure_build.sh baseline clean
#          scripts/measure_build.sh baseline incr src/foo.cpp

PRESET="${1:-baseline}"
PHASE="${2:-clean}"
TOUCH_FILE="${3:-}"

BUILD_DIR="$(jq -r '.configurePresets[] | select(.name=="'"$PRESET"'") | .binaryDir' CMakeUserPresets.json 2>/dev/null || true)"
if [[ -z "$BUILD_DIR" || "$BUILD_DIR" == "null" ]]; then
  echo "Could not resolve build dir for preset $PRESET"; exit 2
fi

mkdir -p "$BUILD_DIR"
LOG_DIR="docs/perf/raw/$(date +%Y%m%d)/$PRESET"
mkdir -p "$LOG_DIR"

if [[ "$PHASE" == "clean" ]]; then
  rm -rf "$BUILD_DIR"
fi

/usr/bin/time -v bash -c "
  cmake --preset=$PRESET &&
  ninja -C $BUILD_DIR -d stats -k 0 all
" \
  > "$LOG_DIR/build_${PHASE}.log" 2> "$LOG_DIR/build_${PHASE}.time"

if [[ "$PHASE" == "incr" && -n "$TOUCH_FILE" ]]; then
  touch "$TOUCH_FILE"
  /usr/bin/time -v ninja -C "$BUILD_DIR" -d stats -k 0 all \
    > "$LOG_DIR/build_${PHASE}_after_touch.log" 2> "$LOG_DIR/build_${PHASE}_after_touch.time"
fi

echo "Logs saved under $LOG_DIR"