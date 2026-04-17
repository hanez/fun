---
layout: post
published: true
author: hanez
author_email: you@hanez.org
author_url: https://hanez.org
noToc: false
title: Announcing Fun 0.40.5
description: Nested functions, Rust integration, and CI automation
date: 2026-04-17
date_updated: 2026-04-17 23:55:00
categories:
- news
- project
- release
tags:
- fun
- programming
- nested-functions
- rust
- automation
---

We are thrilled to announce the release of Fun version 0.40.5! This update represents a major leap forward in language capabilities, developer experience, and project infrastructure since our last major announcement (v0.37.62).

### Key Highlights of Version 0.40.5

#### Nested Functions & Higher-Order Patterns
The most significant language enhancement is the addition of **nested functions** in the Fun parser. This allows for cleaner encapsulation and enables powerful functional programming patterns, such as closures and advanced higher-order function implementations.

<pre>fun outer(x)
  fun inner(y)
    return x + y
  return inner

f = outer(10)
print(f(5))  // 15</pre>

#### Rust & C++ Integration
Fun is getting more "multi-lingual." We've introduced support for implementing opcodes in **Rust**, allowing developers to leverage Rust's safety and performance within the Fun VM. Additionally, we've started evaluating C++ integration for those who need it.

#### Improved Tooling: funstx
We've introduced `funstx`, a dedicated linter for Fun. It helps maintain the "Indentation is Truth" philosophy by ensuring your code follows the strict 2-space indentation rule and other stylistic consistency checks.

#### CI Automation & Reliability
To ensure the stability of the language as it grows, we've implemented a full **GitHub Actions CI workflow**. Every commit now triggers automated execution of our extensive examples library, catching regressions early.

#### VM Configuration & Performance
The Fun VM is now more flexible than ever. Key parameters like `MAX_FRAMES`, `STACK_SIZE`, and `OUTPUT_SIZE` can now be configured at build time via CMake defines (`-D`), allowing you to tune the VM for specific environments (from embedded systems to beefy servers).

### Changelog (v0.37.62 to v0.40.5)

#### Added

- Nested Functions - Full support in the parser (v0.40.0).
- Rust Integration - Build and link Rust-based opcode libraries (v0.38.0).
- GitHub Actions - CI Automated testing and example validation (v0.39.15).
- Linter (funstx) - New tool to enforce code style and indentation (v0.39.0).
- Configurable VM - Build-time tuning of memory limits via CMake (v0.40.5).
- Specification v0.4 - Updated language specification.
- New Stdlib Modules - AES256 support, improved asyncio/scheduler, and more.

#### Changed

- Documentation Relocation - All documentation moved to `./web/documentation/` for better website integration.
- Website Overhaul - The website code was moved to `./web/` for easier maintenance.
- Example Refactoring - Reorganized the `./examples` directory for better clarity and CI integration.
- Code Style - Standardized all C files to two-space indentation.

#### Removed (Cleanup & Focus)

- Removed support for `libsql`, `tcltk`, `notcurses`, and `libressl` to streamline the core and focus on more modern alternatives (v0.40.2 - v0.40.5).

#### Fixed
- Improved line number reporting in error messages (v0.39.15).
- Various bug fixes in the VM and standard library modules.

### The Road to 1.0
As always, Fun remains an experiment, but with version 0.40.5, we are closer than ever to a stable 1.0. We've focused on cleaning up experimental features that didn't fit our minimalism philosophy while adding the powerful primitives needed for real-world hacking.

Want to try it out? Head over to our [Git Repository](https://git.xw3.org/fun/fun){:class="git"} or check out the updated [Documentation](/documentation/).

Happy hacking, and remember: **Fun is Fun!**

Johannes Findeisen (hanez) - [hanez@fun-lang.xyz](mailto:hanez@fun-lang.xyz){:class="mail"}
