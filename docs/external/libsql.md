# libSQL extension (optional)

- CMake option: FUN_WITH_LIBSQL=ON
- Purpose: SQLite-compatible client using the libSQL (Turso) library.
- Homepage: https://libsql.org/

## Opcodes:

- OP_LIBSQL_OPEN: pops url_or_path; pushes handle (>0) or 0
- OP_LIBSQL_CLOSE: pops handle; pushes Nil
- OP_LIBSQL_EXEC: pops sql, handle; pushes rc:int (0=OK)
- OP_LIBSQL_QUERY: pops sql, handle; pushes array<map>

## Notes:

- Uses a SQLite-compatible C API provided by libSQL; behavior is similar to the SQLite backend.
- This module is independent from the SQLite extension; you may enable either or both.
