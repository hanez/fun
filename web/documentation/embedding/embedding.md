---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Embedding Fun
subtitle: Embedding the VM from C/Rust, lifecycle, and host integration tips.
description: Embedding the VM from C/Rust, lifecycle, and host integration tips.
permalink: /documentation/embedding/
lang: en
tags:
- embedding
- host
- integration
- lifecycle
- rust
- tips
---


This guide outlines how to embed the Fun VM in a host application and extend it from C/Rust.

## Overview
- The VM is implemented in C (see [../internals/](../internals/)).
- Optional Rust-based opcodes can be enabled via `FUN_WITH_RUST` (see [../rust/](../rust/)).

## Embedding from C
While the exact API surface may evolve, a typical embedding flow looks like:
1. Initialize the VM/runtime and allocate a context.
2. Load/compile Fun source or bytecode.
3. Push arguments or set globals as needed.
4. Execute entry function or script body.
5. Retrieve results and clean up.

See [https://git.xw3.org/fun/fun/src/branch/main/src/](https://git.xw3.org/fun/fun/src/branch/main/src/){:class="git"} and related headers for public entry points and value types.

### Hosting considerations
- Threading: share VM state cautiously or create one VM per thread.
- Memory: clarify ownership of strings/buffers crossing the boundary.
- Errors: propagate parse/runtime errors back to the host with useful messages.

## Extending with Rust
When `FUN_WITH_RUST=ON`, a Rust static library from [https://git.xw3.org/fun/fun/src/branch/main/src/rust](https://git.xw3.org/fun/fun/src/branch/main/src/rust){:class="git"} is built and linked. You can:
- Implement new opcodes/functions in Rust.
- Expose a C ABI for the VM to call into.

See [../rust/](../rust/) for details and example code.
