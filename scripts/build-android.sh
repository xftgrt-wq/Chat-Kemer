#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NDK="${ANDROID_NDK_HOME:-${ANDROID_NDK_ROOT:-}}"
if [[ -z "$NDK" ]]; then
  echo "ERROR: Set ANDROID_NDK_HOME to your Android NDK directory." >&2
  exit 2
fi
if [[ ! -f "$NDK/build/cmake/android.toolchain.cmake" ]]; then
  echo "ERROR: Android NDK toolchain not found under: $NDK" >&2
  exit 2
fi

BUILD="$ROOT/build-android"
DIST="$ROOT/dist/arm64-v8a/ChatKemer"
cmake -S "$ROOT" -B "$BUILD" \
  -DCMAKE_TOOLCHAIN_FILE="$NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD" --config Release --parallel
rm -rf "$DIST"
mkdir -p "$DIST/config"
cp "$BUILD/libChatKemer.so" "$DIST/libChatKemer.so"
cp "$ROOT/manifest.json" "$DIST/manifest.json"
cp "$ROOT/package/config/config.json" "$DIST/config/config.json"
cp "$ROOT/package/config/config.schema.json" "$DIST/config/config.schema.json"
(cd "$DIST" && zip -qr ../ChatKemer.levipack .)
python3 "$ROOT/scripts/validate-package.py" "$(dirname "$DIST")/ChatKemer.levipack"
echo "Built: $(dirname "$DIST")/ChatKemer.levipack"
