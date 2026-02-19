# External integrations (optional extensions)

This section documents Fun’s optional, build‑time selectable extensions. Each page covers:
- How to enable the extension via CMake option (FUN_WITH_*)
- Build requirements and detection notes
- Available opcodes and/or helper functions
- Minimal usage examples and links to example scripts

Extensions:

- [cURL (libcurl)](./curl.md)
- [INI (iniparser)](./ini.md)
- [JSON (json-c)](./json.md)
- [libxml2 (XML)](./xml2.md)
- [SQLite](./sqlite.md)
- [libSQL (SQLite-compatible)](./libsql.md)
- [PCRE2 (Perl-compatible regex)](./pcre2.md)
- [PC/SC (Smart cards)](./pcsc.md)
- [Notcurses (TUI)](./notcurses.md)
- [Tcl/Tk (GUI)](./tcltk.md)
- [OpenSSL](./openssl.md)

Notes:
- These integrations are optional; the VM compiles without them.
- When disabled, related builtins usually return empty strings/neutral values rather than fail hard, mirroring existing optionality patterns.
