# Fun Documentation Index

This file serves as an index of the documents in this directory. Links are relative and can be opened directly on Git hosting or locally.

## Overview

- [handbook.md](./handbook.md) — Comprehensive handbook for the Fun language and VM: install/build, configuration flags, usage, and full feature overview.
- [types.md](./types.md) — Core types (numbers, strings, arrays, maps, nil/bool), common operations, patterns, and interop notes.
- [arrays.md](./arrays.md) — Working with arrays: creation, indexing/slicing, iteration patterns, helpers, and idioms.
- [maps.md](./maps.md) — Working with maps: construction, lookup/update, merging, iteration, and common patterns.
- [includes.md](./includes.md) — Using local vs. system includes, FUN_LIB_DIR, DEFAULT_LIB_DIR, and namespaced includes with `as`.
- [repl.md](./repl.md) — REPL guide: how to build/launch, editing and history, completions, REPL‑on‑error, and tips.
- [opcodes.md](./opcodes.md) — VM opcodes overview grouped by domain with brief behavior/stack notes.
- [internals.md](./internals.md) — Implementation details: bytecode format, VM architecture, stacks/frames, parser, and dispatch.
- [rust.md](./rust.md) — Writing Rust‑backed opcodes and wiring them into the C VM; build/setup notes.
- [examples.md](./examples.md) — How to run the examples and the interactive showcase script, with environment tips.
- [testing.md](./testing.md) — How to build and run tests/targets with CMake/CTest, and where to add new tests.
- [troubleshooting.md](./troubleshooting.md) — Common issues and quick fixes for build, includes, and REPL usage.

## Tips

- When building from the repo without installing, set `FUN_LIB_DIR` to the local `./lib` directory so examples and the REPL can locate the stdlib.
- For a broader project overview and quickstart, see the repository root [README.md](../README.md).
