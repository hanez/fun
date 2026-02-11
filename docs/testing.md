# Building and Running Tests

This page explains how to build and run Fun’s tests using the existing CMake targets. It also points to where opcode and language tests typically live and how to add new ones.

## Test-related targets

Depending on your generator/profile, you may see the following targets in your IDE or via `cmake --build`:

- fun_test (executable) — main test runner/binary (where present)
- test_opcodes (executable) — opcode-focused tests/demos
- Continuous*/Experimental*/Nightly* — CTest dashboards (optional; mostly for CI)
- build / run / repl — convenience targets for local development

To list targets with CMake directly, consult your IDE or run the build system’s metadata commands. In CLion, targets are shown in the Build tool window.

## Running tests from the command line

Debug profile example:

```
cmake --build build_debug --target test_opcodes && ./build/test_opcodes
```

Release profile example:

```
cmake --build build_release --target test_opcodes && ./build/test_opcodes
``>

If `fun_test` exists in your configuration:

```
cmake --build build_debug --target fun_test && ./build/fun_test
```

You can also invoke CTest to run any tests registered with `add_test()`:

```
cmake --build build_debug --target test
ctest --test-dir build_debug -j
```

## Adding new tests

- C/C++/VM-side tests: look for existing tests under `src` or `spec` and mirror the structure. Add a new source and register it in CMake with an executable or via `add_test()`.
- Fun-level examples as tests: minimal scripts under `examples/` can act as smoke tests and are runnable via `./play.fun`. Consider adding a new example for new features and have CI invoke a subset.

Guidelines:
- Keep each test focused; prefer several small tests over one monolith.
- Avoid nondeterminism; set seeds where randomness is involved.
- Make tests independent of the working directory unless the behavior under test is precisely path resolution.

## Debugging failing tests

- Use `--trace` when running the interpreter to follow execution.
- Enable `--repl-on-error` to inspect state interactively on failure.
- Print intermediate values with `print()` and convert with `to_string()` when needed.

See repl.md and internals.md for deeper debugging tips.
