# Errors and Diagnostics

This guide helps you understand common error messages and how to collect useful diagnostics.

## Common error classes
- Parse errors: syntax/indentation problems detected before execution.
- Name/lookup errors: missing variables, functions, or modules.
- Runtime errors: type mismatches, out-of-range access, invalid operations.

## Enabling diagnostics
- Build with `-DFUN_DEBUG=ON` to enable additional assertions and debug messages (see [build.md](./build.md)).
- Run with smaller, focused scripts to isolate issues.

## Getting useful reports
- Capture the minimal script that reproduces the issue.
- Note your build options and platform.
- Record the full error output from the CLI (`fun`), including line/column if present.

## Tips
- Use prints or logging sparingly to narrow down failing code.
- Start from a passing simple case and add complexity until it breaks.
