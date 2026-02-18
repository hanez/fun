# Bytecode Format (Overview)

This document summarizes the Fun bytecode format. For deep VM details, see [internals.md](./internals.md).

## Goals
- Compact representation for fast loading and dispatch
- Stable header with room for versioning

## File layout (typical)
1. Header
   - Magic number / signature
   - Format version
   - Flags (endianness, feature bits)
   - Offsets to sections
2. Constants table
   - Literals: numbers, strings, compound constants
3. Code section
   - Functions/procedures with instruction streams
   - Line/column mapping (optional for debugging)
4. Auxiliary tables
   - Imports/exports, names, debug info (optional)

## Instructions
- Fixed-size or small-variant opcodes grouped by domain (math, logic, stack, call, control flow, etc.).
- Operands encoded inline following the opcode (width varies by instruction).

## Versioning and compatibility
- The header's version field allows the VM to refuse or translate older/newer formats.
- Keep additions backward-compatible when possible by appending sections or flags.
