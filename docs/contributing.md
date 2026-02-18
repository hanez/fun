# Contributing to Fun

Thanks for your interest in contributing! This guide covers the basics to get you productive quickly.

## Getting started
- Clone the repo and build (see `docs/build.md`).
- Run tests locally (see `docs/testing.md`).
- Explore examples (see `docs/examples.md`).

## Project structure
- `src/` — C core, VM, and opcode implementations (`src/vm/*`).
- `lib/` — Standard library written in Fun.
- `examples/` — Example programs and showcases.
- `docs/` — Documentation.
- `spec/` — Language specification drafts.

## Code style
- C: C99, two-space indent, no tabs. Keep functions short and focused.
- Fun: two-space indent, snake_case for functions, PascalCase for classes/constructors.
- Prefer clear names over abbreviations. See `docs/style-guide.md`.

## Development workflow
1. Create a small, focused branch.
2. Add/adjust tests for behavior changes (see `docs/writing-tests.md`).
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
Be respectful and inclusive. See `CODE_OF_CONDUCT.md` in the repository root.
