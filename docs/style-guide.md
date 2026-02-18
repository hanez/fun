# Fun Style Guide

Conventions for writing C and Fun code in this repository.

## General principles
- Two-space indentation. No tabs.
- Keep lines reasonably short (~100 cols). Wrap thoughtfully.
- Prefer explicit over implicit; favor clarity.

## C (C99)
- Indentation: two spaces, K&R-ish braces.
- Naming: `snake_case` for functions and variables; `CAPS_SNAKE` for macros.
- Error handling: return error codes or booleans; avoid hidden globals.
- Headers: minimize includes in headers; forward-declare where practical.
- Memory: clearly document ownership; free what you allocate.

## Fun language
- Indentation: two spaces.
- Naming: `snake_case` for functions/variables; `PascalCase` for classes/constructors.
- Modules: one primary concept per file; export a minimal, cohesive API.
- Idioms: prefer arrays and maps over ad-hoc structures; keep functions small.

## Formatting and tools
- No auto-formatters required; follow these simple rules.
- Keep diffs small and focused; avoid reformat-only commits.
