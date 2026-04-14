---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - funstx — Fun syntax checker and fixer
subtitle: Syntax checker for .fun files with optional --fix auto-corrections; usage, exit codes, and limitations.
description: Syntax checker for .fun files with optional --fix auto-corrections; usage, exit codes, and limitations.
permalink: /documentation/funstx/
lang: en
tags:
- auto
- checker
- codes
- corrections
- exit
- files
- fix
- fixer
- funstx
- limitations
- optional
- syntax
---

funstx is a small command‑line tool that parses .fun source files to verify syntax without executing them. It lives alongside the fun executable in the build directory and is installed by default.

## Features

- Fast syntax checking for one or many .fun files
- Non‑executing: only parses, never runs code
- Clear error messages with file:line:col
- Optional automatic fixes with `--fix` for common formatting/token issues
- Suitable for batch/CI usage via exit codes

## Usage

<pre>funstx [--fix] [file1.fun] [file2.fun ...]</pre>

- Provide one or more .fun files to check.
- Add `--fix` to attempt safe, automatic corrections before re‑checking.

### Examples

- Check a single file:
  - `funstx examples/arrays.fun`
- Check and auto‑fix a file:
  - `funstx --fix examples/arrays.fun`
- Bulk check and auto‑fix all examples (recursive):
  - `find examples -type f -name '*.fun' -print0 | xargs -0 -n 50 funstx --fix`

## Output

- On success: prints `path/to/file.fun: OK`
- On failure: prints `path/to/file.fun:line:col: syntax error: <message>`

## Exit codes

- `0`: All provided files parsed successfully
- `1`: At least one file failed to parse (after optional fixing)
- `2`: Incorrect usage (e.g., no files provided)

## What `--fix` does

The fixer makes conservative, idempotent edits that align files with parser expectations. It only writes changes if the fixed version parses successfully.

### Applied rules:

- Normalize line endings: CRLF/CR → LF
- Convert leading tabs to spaces; normalize indentation to multiples of two spaces
- Trim trailing spaces
- Ensure the file ends with a single newline
- Normalize type aliases: `sint8|sint16|sint32|sint64` → `int8|int16|int32|int64` (word‑boundary aware)

### Notes:

- Structural issues that require semantic changes aren’t auto‑fixed (e.g., exceeding global limits, missing required delimiters, incomplete statements).
- If parsing still fails after fixing, the original file is left unchanged and an error is reported.

## Build and install

- Built as a regular CMake executable target `funstx` and placed in the same build directory as `fun` (e.g., `build_debug/` or `build_release/`).
- Installed by default alongside `fun` into `/usr/bin` via `make install`/`cmake --build <build_dir> --target install`.

## Integration tips

- Use `funstx --fix` in pre‑commit hooks or CI to enforce consistent formatting.
- For quiet CI logs, capture stdout and only surface stderr on failure.
- Combine with `find`/`xargs` to process large trees efficiently (see examples above).

## Limitations

- Does not execute bytecode or validate runtime behavior.
- The auto‑fixer focuses on formatting and a small set of safe token normalizations; it won’t rewrite program structure.
