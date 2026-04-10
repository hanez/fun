---
layout: page
published: true
noToc: true
noComments: false
noDate: false
title: Fun - Contributing to Fun
subtitle: How to contribute, project structure, coding style, running tests, and PR guidelines.
description: How to contribute, project structure, coding style, running tests, and PR guidelines.
permalink: /documentation/contributing/
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

# Contributing to Fun

Thanks for your interest in contributing! This guide covers the basics to get you productive quickly.

## Getting started
- Clone the repo and build (see [build.md](./build/)).
- Run tests locally (see [testing.md](./testing/)).
- Explore examples (see [examples.md](./examples/)).

## Project structure
- `src/` - C core, VM, and opcode implementations ([src/vm](../src/vm/)).
- `lib/` - Standard library written in Fun.
- `examples/` - Example programs and showcases.
- `documentation/` - Documentation.
- `spec/` - Language specification drafts.

## Code style
- C: C99, two-space indent, no tabs. Keep functions short and focused.
- Fun: two-space indent, snake_case for functions, PascalCase for classes/constructors.
- Prefer clear names over abbreviations. See [style-guide.md](./style-guide/).

## Development workflow
1. Create a small, focused branch.
2. Add/adjust tests for behavior changes (see [writing-tests.md](./writing-tests/)).
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
Be respectful and inclusive. See [CODE_OF_CONDUCT.md](../CODE_OF_CONDUCT/) in the repository root.
