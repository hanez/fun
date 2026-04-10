---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Fun CLI
subtitle: Command-line usage of the `fun` executable, synopsis, options, exit codes, includes and library paths.
description: Command-line usage of the `fun` executable, synopsis, options, exit codes, includes and library paths.
permalink: /documentation/cli/
lang: en
tags:
- cli
- codes
- command
- executable
- exit
- includes
- library
- line
- options
- paths
- synopsis
---


Reference for the `fun` command-line interface.

For a complete usage guide (including REPL details, environment variables, include paths, examples, and install locations), see [fun.md](./fun/).

## Synopsis
```
fun [options] <script.fun> [-- args...]
```

If no script is supplied and interactive mode is available, `fun` starts a REPL (see [repl.md](./repl/)).

## Common options
- `-i`, `--repl` - start an interactive REPL
- `-v`, `--version` - print version and exit
- `-h`, `--help` - show help and exit

Options may vary between versions; run `fun --help` to see what your build supports.

## Exit codes
- `0` - success
- non-zero - error during parse, compile, or runtime

## Includes and library paths
- `FUN_LIB_DIR` - environment variable that points to the stdlib location; when running from the repo, set this to `./lib`.
- `DEFAULT_LIB_DIR` - compiled-in fallback path determined at build/install time.

See also: [includes.md](./includes/) for namespaced includes and search order.

## Examples
Run a script:
```
FUN_LIB_DIR=./lib ./build/fun examples/hello.fun
```

Start the REPL:
```
./build/fun -i
```
