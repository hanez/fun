---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Running the Examples
subtitle: How to run the examples and the interactive showcase script, with environment tips.
description: How to run the examples and the interactive showcase script, with environment tips.
permalink: /documentation/examples/
lang: en
tags:
- environment
- example
- examples
- interactive
- run
- running
- script
- showcase
- tips
---


This page shows how to run the example programs included with the repository and how to use the interactive showcase script.

All commands assume you are in the repository root.

## Prerequisites

- Build the interpreter (see handbook/). You’ll have `build/fun` (paths may vary by your setup/IDE).
- Set FUN_LIB_DIR to the repo’s lib directory when running without installation so `#include <...>` can find the standard library.

Example (Linux/macOS/BSD):

<pre>FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/include_lib.fun
</pre>
Windows (PowerShell):

<pre>$env:FUN_LIB_DIR = "$PWD/lib"
./build/fun.exe .\examples\include_lib.fun
</pre>
## Interactive showcase: play.fun

The script `./play.fun` discovers all `.fun` files under `./examples` and offers to run them one by one:

<pre>./play.fun
</pre>
Notes:
- The script auto-picks your interpreter (FUN_BIN env or `fun` in PATH) and ensures `FUN_LIB_DIR=./lib` so examples resolve includes correctly.
- It shows the exit code for each run and summarizes failures at the end.

Tip: you can run specific examples directly too:

<pre>FUN_LIB_DIR="$(pwd)/lib" fun examples/crypto/openssl_md5.fun
</pre>
## Example categories

Browse the `examples/` tree for areas of interest:

- crypto — crypto demonstrations (e.g., OpenSSL MD5/SHA-256/SHA-512 helpers; requires build with `-DFUN_WITH_OPENSSL=ON`)
 - crypto — crypto demonstrations (e.g., OpenSSL MD5/SHA-256/SHA-512/RIPEMD‑160 helpers; requires build with `-DFUN_WITH_OPENSSL=ON`)
- blocking / interactive — I/O or user-interactive patterns
- error / broken — negative tests and error showcases
- math — numeric operations
- sqlited / data — data access and HTTP/CGI-style samples (platform dependent)

## Creating your own examples

Place your `.fun` files anywhere under `examples/` to have them picked up by `play.fun`. Use quoted includes for project-local helpers and angle brackets for stdlib modules:

<pre>#include "examples/my_lib/common.fun"
#include <io/console.fun>
</pre>
If you add an example showcasing a new feature, also consider adding a brief note to the relevant doc (types.md, includes.md, opcodes.md, etc.).
