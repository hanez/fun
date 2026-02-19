# SQLite extension (optional)

- CMake option: FUN_WITH_SQLITE=ON
- Purpose: Access SQLite databases via the native C API.
- Homepage: https://www.sqlite.org/

## Opcodes:

- OP_SQLITE_OPEN: pops path; pushes handle (>0) or 0
- OP_SQLITE_CLOSE: pops handle; pushes Nil
- OP_SQLITE_EXEC: pops sql, handle; pushes rc:int (0=OK)
- OP_SQLITE_QUERY: pops sql, handle; pushes array<map>

## Notes:

- Requires SQLite development headers/libs.
- See also: `libSQL` for a compatible alternative backend.
