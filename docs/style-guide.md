# Fun Style Guide

Conventions for writing C and Fun code in this repository.

## General principles

- Two-space indentation. No tabs.
- Keep lines reasonably short (~100 cols). Wrap thoughtfully.
- Prefer explicit over implicit; favor clarity.

## C (C99)

- Indentation: four spaces, K&R-ish braces.
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

## Additional details (merged from legacy MVP draft)

This section consolidates practical guidelines that were previously kept in docs/style_guide.md.

1. Files and headers
- Keep the standard header block with license and date when editing stdlib files.

2. Naming
- Functions and variables: snake_case (e.g., parse_int, is_some).
- Classes: PascalCase (e.g., DateTime).
- Constants: ALL_CAPS when truly constant.

3. Layout
- Indent with two spaces; no tabs.
- One statement per line; no trailing spaces.
- Use blank lines sparingly to separate logical blocks.

4. Comments
- Line comments with //; block comments with /* ... */ for file headers and longer notes.

5. Collections
- Prefer [] for arrays and {"key": value} for dictionaries; use has_key() before subscripting unknown keys.

6. Error handling
- Prefer Result and Option helpers from lib/utils over ad-hoc nil checks.
- Avoid unwrap() in library code; propagate errors using and_then()/or_else() patterns.

7. Examples
- Keep examples executable via shebang: #!/usr/bin/env fun
- Print informative labels for outputs.

This guide will evolve. Contributions and suggestions are welcome.
