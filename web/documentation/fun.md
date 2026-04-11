---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Fun executable: full usage guide
subtitle: Full usage guide for the `fun` executable, invocation patterns, REPL, env vars, include paths, examples, and install locations.
description: Full usage guide for the `fun` executable, invocation patterns, REPL, env vars, include paths, examples, and install locations.
permalink: /documentation/fun/
lang: en
tags:
- env
- examples
- executable
- full
- include
- install
- invocation
- locations
- paths
- patterns
- vars
---


This document explains how to use the `fun` binary after building or installing it: invocation patterns, options, environment variables, include/search paths, REPL, and examples.

## Synopsis
<pre>fun [options] [<script.fun>] [-- args...]
</pre>
- If `<script.fun>` is provided, `fun` runs the script.
- If omitted and the build enables the REPL, `fun` starts an interactive session.

## Options
- `-i`, `--repl` — start the interactive REPL explicitly (if built with REPL support)
- `-v`, `--version` — print version and exit
- `-h`, `--help` — show usage info and exit

Notes:
- Available options may vary by build configuration. Always check `fun --help` for your binary.

## Exit codes
- `0` — success
- non‑zero — error during parse, compile, or runtime

## Standard library and includes
`fun` searches for modules included from `.fun` code in these locations:

1. The path provided by the environment variable `FUN_LIB_DIR` (highest precedence)
2. The compiled‑in default `DEFAULT_LIB_DIR` (configured during build/install)

Tips:
- When running from the repository without installing, set `FUN_LIB_DIR` to the local `./lib` directory so examples can find the stdlib.
- See also: `documentation/includes.md` for namespacing (`include ... as ...`) and search order details.

## REPL
If interactive mode is enabled at build time, starting `fun` with `-i` (or without a script) opens the REPL.

Useful commands/patterns in REPL:
- Enter expressions and statements directly.
- Use arrow keys/history for editing (availability depends on build flags).
- See `documentation/repl.md` for details and tips.

## Running scripts
Basic run (installed system‑wide):
<pre>fun /usr/share/fun/examples/hello.fun
</pre>
Running from a build tree (not installed):
<pre>FUN_LIB_DIR=./lib /path/to/build_dir/fun examples/hello.fun
</pre>
Passing arguments to scripts (arguments after `--` are forwarded to the script environment):
<pre>fun myscript.fun -- arg1 arg2
</pre>
## Build and install locations
- Build targets: `fun` is produced by the `fun` target. In CLion/CMake, typical build directories are `build_debug` or `build_release`.
- Install locations (by default):
  - Binary: `/usr/bin/fun`
  - Stdlib: `/usr/share/fun/lib`
  - Examples (optional): `/usr/share/fun/examples`

To stage an install without touching the system:
<pre>DESTDIR=./tmp/stage cmake --build <build_dir> --target install
./tmp/stage/usr/bin/fun ./examples/hello.fun
</pre>
## See also
- `documentation/cli.md` — concise CLI reference (synopsis/options/exit codes)
- `documentation/funstx.md` — syntax checker for `.fun` files with optional `--fix`
- `documentation/examples.md` — how to run the bundled examples
- `documentation/includes.md` — include paths, namespacing, and `FUN_LIB_DIR`
