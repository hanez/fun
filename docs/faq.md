# FAQ

Answers to common questions.

## I built Fun but includes aren’t found
Set `FUN_LIB_DIR` to the repository’s `./lib` directory when running without installation:
```
FUN_LIB_DIR=./lib ./build_debug/fun examples/hello.fun
```
See `docs/includes.md`.

## How do I start the REPL?
Run `fun -i` (or run `fun` without a script, depending on version). See `docs/repl.md`.

## Which build target should I use?
Use the aggregate `build` target to build `fun`, `fun_test`, and `test_opcodes`. See `docs/build.md`.

## Where are the standard libraries?
Under `./lib/`. See `docs/stdlib.md` for an overview.

## Where can I find internals and opcodes?
Browse `src/vm/` and `docs/internals.md` / `docs/opcodes.md`.
