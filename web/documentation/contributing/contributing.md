---
layout: page
published: true
noToc: false
noComments: false
noDate: false
title: Fun - Contributing to Fun
subtitle: How to contribute, project structure, coding style, running tests, and PR guidelines.
description: How to contribute, project structure, coding style, running tests, and PR guidelines.
permalink: /documentation/contributing/
lang: en
tags:
- coding
- contribute
- contributing
- guidelines
- project
- running
- structure
- style
- tests
---


Thanks for your interest in contributing! This guide covers the basics to get you productive quickly.

## Getting started
- Clone the repo and build (see [../build/](../build/)).
- Run tests locally (see [../testing/](../testing/)).
- Explore examples (see [../examples/](../examples/)).

## Project structure
- `src/` - C core, VM, and opcode implementations ([https://git.xw3.org/fun/fun/src/branch/main/src/](https://git.xw3.org/fun/fun/src/branch/main/src/){:class="git"}).
- `lib/` - Standard library written in Fun.
- `web/` - Website and documentation ([https://git.xw3.org/fun/fun/src/branch/main/web/](https://git.xw3.org/fun/fun/src/branch/main/web/){:class="git"}).
- `examples/` - Example programs and showcases.
- `spec/` - Language specification drafts.
- `cmake/` - CMake configuration and modules.
- `scripts/` - Utility scripts for development.
- `make` - Helper script for building (wraps CMake).

## Code style
- C: C99, two-space indent, no tabs. Keep functions short and focused.
- Fun: two-space indent, snake_case for functions, PascalCase for classes/constructors.
- Prefer clear names over abbreviations. See [../style-guide/](../style-guide/).

## Development workflow
1. Create a small, focused branch.
2. Add/adjust tests for behavior changes (see [../writing-tests](../writing-tests/)).
3. Update docs if user-visible behavior changes.
4. Submit a PR with a clear description and rationale.

## Commit/PR guidelines
- Keep commits atomic; include test updates with the change.
- Reference related issues.
- Include benchmarks only when meaningful and reproducible.

## Reporting bugs
Please include:
- Reproducer script (minimal), expected vs. actual behavior
- Build options and platform
- `fun --version` output

## Code of Conduct
Be respectful and inclusive. See [https://git.xw3.org/fun/fun/src/branch/main/CODE_OF_CONDUCT.md](https://git.xw3.org/fun/fun/src/branch/main/CODE_OF_CONDUCT/){:class="git"} in the repository root.
