# Performance Guide

Tips for getting good performance from Fun programs and builds.

## Build configuration
- Use `-DCMAKE_BUILD_TYPE=Release` for production binaries.
- Consider `FUN_DEBUG=OFF` to remove extra checks in hot paths.
- `FUN_USE_MUSL=ON` can help with static/portable builds; measure performance for your use case.

## Language-level tips
- Prefer pre-sized arrays/maps when possible to reduce reallocations.
- Avoid unnecessary string concatenations in loops; accumulate in arrays and `join`.
- Push work to native ops where available (e.g., regex, JSON, math helpers).

## Algorithmic choices
- Choose appropriate data structures (arrays for ordered scans, maps for key lookups).
- Cache repeated computations or lookups when safe.

## Measuring
- Create small, representative benchmarks.
- Compare Debug vs. Release to ensure changes are meaningful.
