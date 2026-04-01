Fun SQL TCP Demo (sqlited)

This example provides a minimal TCP server that executes SQL against a local SQLite database and a matching client.

Files
- server.fun — TCP server daemon
- client.fun — simple CLI client
- protocol.md — wire protocol specification (line-based TSV)

Prerequisites
- Build Fun with SQLite support enabled: configure with -DFUN_WITH_SQLITE=ON
- Ensure the sqlite3 development headers and runtime are installed

Create a sample database
- A schema is provided at examples/data/database.sql
- Create ./database.sqlite at the repository root using the sqlite3 CLI:
  sqlite3 ./database.sqlite < ./examples/data/database.sql

Run the server
- Set FUN_LIB_DIR to the repo’s lib directory or install Fun libs system-wide
- Example (Debug profile path may differ):
  FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlited/server.fun 127.0.0.1 5555

Run the client
- Query:
  FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlited/client.fun 127.0.0.1 5555 "SELECT id, title FROM tasks;"
- Exec/DDL:
  FUN_LIB_DIR="$(pwd)/lib" ./build/fun ./examples/sqlited/client.fun 127.0.0.1 5555 "UPDATE tasks SET done=1 WHERE id=1;"

Protocol summary
- Client sends one line with SQL ended by a newline (\n)
- Server responds with either:
  - RESULT block (header + rows as TSV) ending with END
  - OK rc (for exec/DDL)
  - ERROR message (on error)
See protocol.md for details.

Notes and limitations
- Demo only; do not expose to untrusted networks (no auth/TLS; arbitrary SQL)
- BLOBs and binary data are not specially handled in this v1
- Very long SQL lines are capped at 64 KiB
- The server handles one client at a time (simple model); extend with threads if desired
