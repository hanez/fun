#!/usr/bin/env bash
set -euo pipefail

# This file is part of the Fun programming language.
# https://fun-lang.xyz/
#
# Copyright 2025 Johannes Findeisen <you@hanez.org>
# Licensed under the terms of the Apache-2.0 license.
# https://opensource.org/license/apache-2-0

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
EX_DIR="$ROOT/examples"

# Optional overrides via FUN_BIN env. First positional arg may now be an examples subdirectory.
BIN="${FUN_BIN:-}"
SUBDIR="${1:-}"

pick_bin() {
  local cands=(
    "$ROOT/fun"
    "$ROOT/build/fun"
    "$ROOT/build/bin/fun"
    "$ROOT/cmake-build-debug/fun"
    "$ROOT/cmake-build-release/fun"
  )
  for p in "${cands[@]}"; do
    [[ -x "$p" ]] && { echo "$p"; return 0; }
  done
  # Last resort: search shallowly for an executable named 'fun' under common build dirs
  local found
  found="$(
    find "$ROOT" -maxdepth 3 -type f -name fun -perm -u+x 2>/dev/null \
      | grep -E '/(build|cmake-build[^/]*)/fun$' \
      | head -n 1 || true
  )"
  [[ -n "$found" ]] && { echo "$found"; return 0; }
  return 1
}

# If user passed a subdir that actually looks like an executable path, treat it as BIN for backward compatibility
if [[ -n "$SUBDIR" && -x "$SUBDIR" && ! -d "$EX_DIR/$SUBDIR" ]]; then
  BIN="$SUBDIR"
  SUBDIR=""
fi

if [[ -z "${BIN}" ]]; then
  if ! BIN="$(pick_bin)"; then
    echo "error: fun binary not found. Try building it (e.g., via CMake) or pass it explicitly:" >&2
    echo "  FUN_BIN=/path/to/fun scripts/run_examples.sh [examples-subdir]" >&2
    echo "or: scripts/run_examples.sh /path/to/fun" >&2
    exit 2
  fi
fi

if [[ ! -x "$BIN" ]]; then
  echo "error: fun binary not found or not executable at $BIN" >&2
  exit 2
fi

# Ensure stdlib is discoverable for examples unless user already set it
if [[ -z "${FUN_LIB_DIR:-}" ]]; then
  export FUN_LIB_DIR="$ROOT/lib"
fi

# Do not auto-move failing examples; keep user's workspace unchanged

# Determine target examples directory (optionally restricted to a subdir)
TARGET_DIR="$EX_DIR"
if [[ -n "${SUBDIR}" ]]; then
  if [[ -d "$EX_DIR/$SUBDIR" ]]; then
    TARGET_DIR="$EX_DIR/$SUBDIR"
  else
    echo "error: examples subdirectory not found: $SUBDIR" >&2
    echo "       expected at: $EX_DIR/$SUBDIR" >&2
    exit 2
  fi
fi

shopt -s nullglob
files=("$TARGET_DIR"/*.fun)
shopt -u nullglob

if (( ${#files[@]} == 0 )); then
  echo "No .fun example files found in $TARGET_DIR"
  exit 0
fi

rc=0
fail_count=0
for f in "${files[@]}"; do
  base_name="$(basename "$f")"
  # If INI support is disabled, skip INI examples to avoid expected failures
  if [[ "${FUN_WITH_INI:-}" =~ ^(0|OFF|off|false|False)$ ]]; then
    if [[ "$base_name" == ini_*.fun ]]; then
      echo "=== Skipping (INI disabled): examples/$base_name ==="
      continue
    fi
  fi
  echo "=== Running: ${f#$ROOT/} ==="
  if ! "$BIN" "$f"; then
    echo "FAILED: ${f#$ROOT/}"
    # Intentionally do not move files on failure; leave control to the user
    rc=1
    ((fail_count++))
  fi
done

if (( rc == 0 )); then
  echo "All examples ran successfully. Failed: ${fail_count}"
else
  echo "Some examples failed. Failed: ${fail_count}"
fi
exit $rc
