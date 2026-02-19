# JSON (json-c) extension (optional)

- CMake option: FUN_WITH_JSON=ON
- Purpose: JSON parse/stringify and file helpers via json-c.
- Homepage: https://json-c.github.io/json-c/

Opcodes:
- OP_JSON_PARSE: pops text; pushes value or Nil on error
- OP_JSON_STRINGIFY: pops pretty:int(0/1), value; pushes string
- OP_JSON_FROM_FILE: pops path; pushes value or Nil
- OP_JSON_TO_FILE: pops pretty:int(0/1), value, path; pushes 1/0

Notes:
- Requires json-c development headers/libs.
- When disabled, functions push empty/neutral values similar to other optional modules.
