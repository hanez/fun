---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - FAQ
subtitle: Frequently asked questions and quick answers.
description: Frequently asked questions and quick answers.
permalink: /documentation/faq/
lang: en
tags:
- documentation
- handbook
- installation
- usage
- introduction
- help
- guide
- howto
- docs
- specifications
- specs
- repl
---

# FAQ

Answers to common questions.

## I built Fun but includes aren't found
Set `FUN_LIB_DIR` to the repository's `./lib` directory when running without installation:
```
FUN_LIB_DIR=./lib ./build/fun examples/hello.fun
```
See [includes.md](./includes/).

## How do I start the REPL?
Run `fun -i` (or run `fun` without a script, depending on version). See [repl.md](./repl/).

## Which build target should I use?
Use the aggregate `build` target to build `fun`, `fun_test`, and `test_opcodes`. See [build.md](./build/).

## Where are the standard libraries?
Under [`./lib/`](../lib/). See [stdlib.md](./stdlib/) for an overview.

## Where can I find internals and opcodes?
Browse [src/vm](../src/vm/) and [internals.md](./internals/) / [opcodes.md](./opcodes/).
