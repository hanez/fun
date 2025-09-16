#!/usr/bin/env bash
set -euo pipefail

# This file is part of the Fun programming language.
# https://hanez.org/project/fun/
#
# Copyright 2025 Johannes Findeisen <you@hanez.org>
# Licensed under the terms of the ISC license.
# https://opensource.org/license/isc-license-txt

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
EX_DIR="$ROOT/examples"

# Allow override via first arg or FUN_BIN env
BIN="${1:-${FUN_BIN:-}}"

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

if [[ -z "${BIN}" ]]; then
  if ! BIN="$(pick_bin)"; then
    echo "error: fun binary not found. Try building it (e.g., via CMake) or pass it explicitly:" >&2
    echo "  scripts/run_examples.sh /path/to/fun" >&2
    echo "or set FUN_BIN=/path/to/fun" >&2
    exit 2
  fi
fi

if [[ ! -x "$BIN" ]]; then
  echo "error: fun binary not found or not executable at $BIN" >&2
  exit 2
fi

shopt -s nullglob
files=("$EX_DIR"/*.fun)
shopt -u nullglob

if (( ${#files[@]} == 0 )); then
  echo "No .fun example files found in $EX_DIR"
  exit 0
fi

rc=0
for f in "${files[@]}"; do
  echo "=== Running: ${f#$ROOT/} ==="
  if ! "$BIN" "$f"; then
    echo "FAILED: ${f#$ROOT/}"
    rc=1
  fi
done

if (( rc == 0 )); then
  echo "All examples ran successfully."
else
  echo "Some examples failed."
fi
exit $rc
