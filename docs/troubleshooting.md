# Troubleshooting Guide

This page lists common issues when building and running Fun from a source checkout and how to fix them quickly.

## Includes cannot be found

Error example:

```
Include error: cannot read '<io/console.fun>'
```

Fix:
- When running from the repository without installing, set `FUN_LIB_DIR` to the local `./lib` directory so angle‑bracket includes resolve correctly.

Linux/macOS/BSD:

```
export FUN_LIB_DIR="$(pwd)/lib"
./build/fun examples/include_lib.fun
```

Windows (PowerShell):

```
$env:FUN_LIB_DIR = "$PWD/lib"
./build/fun.exe .\examples\include_lib.fun
```

If `FUN_LIB_DIR` is not set, the interpreter tries a compile‑time `DEFAULT_LIB_DIR`, and finally falls back to `./lib` relative to the current working directory. Be mindful of where you run the `fun` binary from.

See includes.md for more details.

## REPL does not start

Symptoms:
- Running the executable just exits, or `--repl-on-error` is not recognized.

Fix:
- Build with `-DFUN_WITH_REPL=ON` and rebuild the `fun` target. Then launch without arguments:

```
cmake -S . -B build -DFUN_WITH_REPL=ON
cmake --build build --target fun
FUN_LIB_DIR="$(pwd)/lib" ./build/fun
```

See repl.md for usage tips and features.

## Linker errors for optional libraries (JSON, PCRE2, CURL, SQLite, etc.)

Symptoms:
- Build fails when enabling an optional feature.

Fix:
- Ensure the development packages for the chosen library are installed (headers + libs).
- Toggle features individually with `-DFUN_WITH_JSON=ON`, `-DFUN_WITH_PCRE2=ON`, etc., and verify your system provides them.

## Bytecode or opcode mismatch errors after refactors

Symptoms:
- Crashes or incorrect behavior after editing src/vm or src/bytecode.h.

Fix:
- Rebuild cleanly to ensure all amalgamated C units are recompiled.
- Verify opcode_names[] in src/vm.h matches enum ordering in src/bytecode.h.
- Re-run with `--trace` and (optionally) `--dump-bytecode` to inspect control flow.

## Paths differ when running from IDE vs. shell

Symptoms:
- Includes resolve in one environment but not the other; `./lib` fallback behaves differently.

Fix:
- Always set `FUN_LIB_DIR` explicitly in the run configuration of your IDE and in your shell session.
- Confirm the working directory of the launch configuration; relative includes use `$PWD`.

## Windows-specific quoting issues

Symptoms:
- Setting environment variables has no effect; include paths remain unresolved.

Fix:
- In CMD use: `set FUN_LIB_DIR=%CD%\lib && build\fun.exe examples\include_lib.fun`
- In PowerShell use: `$env:FUN_LIB_DIR = "$PWD\lib"; .\build\fun.exe .\examples\include_lib.fun`

## Getting help

- Run the interpreter with `--help` to see supported flags.
- Explore docs/handbook.md and docs/repl.md.
- Check the examples under `examples/` for an interactive tour.
