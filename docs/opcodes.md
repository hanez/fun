# Fun VM Opcodes Overview

This document provides an overview of the available VM opcodes implemented under src/vm and its subdirectories. Opcodes are grouped by module. For each opcode, we briefly note its purpose and, where available, the expected stack arguments and return value.

## Notes

- Arguments are pushed on the stack before the opcode executes; results are pushed back on the stack.
- Types are indicative; many opcodes accept integers and/or strings depending on context. See code for exact type checks and error behavior.

## Core

- OP_NOP: No operation.
- OP_HALT: Stop the VM.
- OP_EXIT: Terminate script with an exit code; pops int exitCode.
- OP_LOAD_CONST: Push a constant (by index embedded in bytecode).
- OP_LOAD_LOCAL: Push local variable value (by slot index).
- OP_STORE_LOCAL: Pop value and store into local (by slot index).
- OP_LOAD_GLOBAL: Push global variable value (by slot index/name id).
- OP_STORE_GLOBAL: Pop value and store into global (by slot index/name id).
- OP_CALL: Call a function; pops N args and a callable; pushes return value.
- OP_RETURN: Return from current function; optionally pops return value and pushes it to caller frame.
- OP_POP: Pop and discard the top of the stack.
- OP_DUP: Duplicate the top stack value.
- OP_SWAP: Swap the top two stack values.
- OP_JUMP: Unconditional jump to bytecode offset.
- OP_JUMP_IF_FALSE: Pop condition; jump if falsey.
- OP_THROW: Pop error and raise; unwinds to nearest try.
- OP_TRY_PUSH: Begin try handler (internal to exception handling).
- OP_TRY_POP: End try handler (internal to exception handling).

## Arithmetic

- OP_ADD: Add two numbers or concatenate two strings; pops b, a; pushes a+b or a..b.
- OP_SUB: Integer subtraction; pops b, a; pushes a-b.
- OP_MUL: Integer multiplication; pops b, a; pushes a*b.
- OP_DIV: Integer division; pops b, a; pushes a/b.
- OP_MOD: Integer modulus; pops b, a; pushes a%b. (math/mod.c)

## Logic and Comparison

- OP_NOT: Logical NOT; pops v; pushes bool.
- OP_AND: Logical AND; pops b, a; pushes bool.
- OP_OR: Logical OR; pops b, a; pushes bool.
- OP_EQ: Equality; pops b, a; pushes bool.
- OP_NEQ: Inequality; pops b, a; pushes bool.
- OP_LT: Less than; pops b, a; pushes bool.
- OP_LTE: Less than or equal; pops b, a; pushes bool.
- OP_GT: Greater than; pops b, a; pushes bool.
- OP_GTE: Greater than or equal; pops b, a; pushes bool.

## Arrays and Indexing

- OP_MAKE_ARRAY: Create array from N values; pops N values (count encoded in bytecode); pushes array.
- OP_INDEX_GET: Indexing into array or map; pops index/key, container; pushes value or Nil on missing.
- OP_INDEX_SET: Assign into array or map; pops value, index/key, container; pushes 1/0 for success (see code for exact behavior).
- OP_INSERT: Insert value at index in array; pops value, index, array; pushes 1/0.
- OP_REMOVE: Remove element at index in array; pops index, array; pushes removed value or Nil.
- OP_PUSH: Append element to end of array; pops value, array; pushes 1/0.
- OP_APOP: Pop last element from array; pops array; pushes removed value or Nil.
- OP_SLICE: Slice array; pops end, start, array; pushes new array slice.
- OP_INDEX_OF: Find index of value in array; pops value, array; pushes index or -1.
- OP_CONTAINS: Membership test for arrays; pops value, array; pushes bool.
- OP_JOIN: Join array of strings; pops separator, array; pushes string.
- OP_CLEAR: Clear all elements in array; pops array; pushes 1/0.
- OP_ENUMERATE: Produce array of [index, value] pairs from array; pops array; pushes array of pairs.

## Maps

- OP_MAKE_MAP: Create a map from N key-value pairs; pops 2*N values (val, key ...); pushes map.
- OP_HAS_KEY: Check if key exists; pops key, map; pushes bool.
- OP_KEYS: Return array of keys; pops map; pushes array.
- OP_VALUES: Return array of values; pops map; pushes array.

## Strings and Regex

- OP_SUBSTR: Substring; pops len, start, string; pushes substring.
- OP_FIND: Find substring; pops needle, haystack; pushes index or -1.
- OP_SPLIT: Split string; pops separator, string; pushes array of strings.
- OP_REGEX_MATCH: Regex full match; pops pattern, string; pushes bool or captures (see strings/regex_match.c).
- OP_REGEX_SEARCH: Regex search/find; pops pattern, string; pushes match details or -1.
- OP_REGEX_REPLACE: Regex replace; pops replacement, pattern, string; pushes new string.

## Bitwise (uint32)

- OP_BAND: Bitwise AND; pops b, a; pushes a & b.
- OP_BOR: Bitwise OR; pops b, a; pushes a | b.
- OP_BXOR: Bitwise XOR; pops b, a; pushes a ^ b.
- OP_BNOT: Bitwise NOT; pops a; pushes ~a.
- OP_SHL: Logical left shift; pops shift, value; pushes value << shift.
- OP_SHR: Logical right shift; pops shift, value; pushes value >> shift.
- OP_ROTL: Rotate left; pops shift, value; pushes rotl(value, shift).
- OP_ROTR: Rotate right; pops shift, value; pushes rotr(value, shift).

## Math

- OP_ABS: Absolute value; pops x; pushes |x|.
- OP_CEIL: Ceiling; pops x; pushes ceil(x).
- OP_FLOOR: Floor; pops x; pushes floor(x).
- OP_ROUND: Round; pops x; pushes round(x).
- OP_TRUNC: Truncate; pops x; pushes trunc(x).
- OP_SIN: Sine; pops x; pushes sin(x).
- OP_COS: Cosine; pops x; pushes cos(x).
- OP_TAN: Tangent; pops x; pushes tan(x).
- OP_SQRT: Square root; pops x; pushes sqrt(x) (float).
- OP_ISQRT: Integer square root; pops x; pushes isqrt(x) (int).
- OP_LOG: Natural logarithm; pops x; pushes log(x).
- OP_LOG10: Base-10 logarithm; pops x; pushes log10(x).
- OP_EXP: Exponential; pops x; pushes exp(x).
- OP_MAX: Max of two ints; pops b, a; pushes max(a, b).
- OP_MIN: Min of two ints; pops b, a; pushes min(a, b).
- OP_FMAX: Max of two floats; pops b, a; pushes max(a, b).
- OP_FMIN: Min of two floats; pops b, a; pushes min(a, b).
- OP_CLAMP: Clamp; pops hi, lo, x; pushes clamp(x, lo, hi).
- OP_SIGN: Sign; pops x; pushes -1, 0, or 1.
- OP_POW: Power; pops exp, base; pushes base^exp.
- OP_GCD: Greatest common divisor; pops b, a; pushes gcd(a, b).
- OP_LCM: Least common multiple; pops b, a; pushes lcm(a, b).
- OP_RANDOM_SEED: Seed RNG; pops int seed; pushes 1/0.
- OP_RANDOM_INT: Random integer in [lo, hi]; pops hi, lo; pushes int.

## I/O

- OP_READ_FILE: Read file contents; pops path:string; pushes data:string or Nil.
- OP_WRITE_FILE: Write data to file; pops data:string, path:string; pushes 1/0.
- OP_INPUT_LINE: Read a line from stdin; optional prompt on stack; pushes string (may be empty) or Nil.

## JSON

- OP_JSON_PARSE: Parse JSON text; pops text:string; pushes value (map/array/number/string/bool/Nil) or Nil on error.
- OP_JSON_STRINGIFY: Stringify a value; pops pretty:int(0/1), any; pushes json:string.
- OP_JSON_TO_FILE: Write value as JSON to file; pops pretty:int(0/1), any, path; pushes 1/0.
- OP_JSON_FROM_FILE: Read and parse JSON file; pops path; pushes value or Nil.

## XML

- OP_XML_PARSE: Parse XML text; pops text:string; pushes doc handle (>0) or 0 on error.
- OP_XML_ROOT: Get root node; pops doc handle; pushes node handle (>0) or 0.
- OP_XML_NAME: Get node name; pops node handle; pushes string.
- OP_XML_TEXT: Get node text (concatenated); pops node handle; pushes string.

## INI

- OP_INI_LOAD: Load INI; pops path:string; pushes handle (>0) or 0.
- OP_INI_FREE: Free INI handle; pops handle; pushes 1/0.
- OP_INI_GET_STRING: Get string; pops default, key, section, handle; pushes string.
- OP_INI_GET_INT: Get integer; pops default, key, section, handle; pushes int.
- OP_INI_GET_DOUBLE: Get double; pops default, key, section, handle; pushes float.
- OP_INI_GET_BOOL: Get bool; pops default, key, section, handle; pushes 1/0.
- OP_INI_SET: Set value; pops value, key, section, handle; pushes 1/0.
- OP_INI_UNSET: Remove key; pops key, section, handle; pushes 1/0.
- OP_INI_SAVE: Save to file; pops path, handle; pushes 1/0.

## SQLite

- OP_SQLITE_OPEN: Open database; pops path:string; pushes handle (>0) or 0.
- OP_SQLITE_CLOSE: Close database; pops handle; pushes Nil.
- OP_SQLITE_EXEC: Execute statement; pops handle:int, sql:string; pushes rc:int (0=OK).
- OP_SQLITE_QUERY: Run query; pops handle:int, sql:string; pushes array<map<string,any>>.

## libSQL

- OP_LIBSQL_OPEN: Open database (url or path); pops string; pushes handle (>0) or 0.
- OP_LIBSQL_CLOSE: Close database; pops handle; pushes Nil.
- OP_LIBSQL_EXEC: Execute statement; pops handle:int, sql:string; pushes rc:int (0=OK).
- OP_LIBSQL_QUERY: Run query; pops handle:int, sql:string; pushes array<map<string,any>>.

## OS, Time, Processes, Threads, Sockets, Serial

- OP_ENV: Get environment variable; pops key:string; pushes value:string or Nil.
- OP_ENV_ALL: Get all environment variables; pushes map<string,string>.
- OP_FUN_VERSION: Push Fun version string; no args.
- OP_SLEEP_MS: Sleep; pops ms:int; pushes Nil.
- OP_TIME_NOW_MS: Current wall-clock time in ms since epoch; pushes int.
- OP_CLOCK_MONO_MS: Monotonic clock in ms; pushes int.
- OP_DATE_FORMAT: Format epoch ms with strftime; pops format:string, ms:int; pushes string.
- OP_RANDOM_NUMBER: Random float in [0,1); optional lower/upper bound handling; see os/random_number.c.
- OP_PROC_SYSTEM: Run command via system(); pops cmd:string; pushes exit code:int.
- OP_PROC_RUN: Run command and capture stdout/stderr; pops cmd:string; pushes map or string (see os/proc_run.c).
- OP_LIST_DIR / OP_OS_LIST_DIR: List directory; pops path; pushes array of file names.
- Threads:
  - OP_THREAD_SPAWN: Spawn a thread to run a function; pops args (array or scalar), fn; pushes thread id.
  - OP_THREAD_JOIN: Join thread; pops thread id; pushes thread result.
- Sockets (TCP/Unix):
  - OP_SOCK_TCP_LISTEN: Listen on TCP port; pops backlog:int, port:int; pushes fd:int or -1.
  - OP_SOCK_TCP_ACCEPT: Accept a connection; pops fd:int; pushes client fd:int or -1.
  - OP_SOCK_TCP_CONNECT: Connect to host:port; pops port:int, host:string; pushes fd:int or -1.
  - OP_SOCK_UNIX_LISTEN: Listen on Unix domain socket; pops backlog:int, path:string; pushes fd:int or -1.
  - OP_SOCK_UNIX_CONNECT: Connect to Unix domain socket; pops path:string; pushes fd:int or -1.
  - OP_SOCK_SEND: Send bytes; pops data:string, fd:int; pushes bytesSent:int or -1.
  - OP_SOCK_RECV: Receive bytes; pops max:int, fd:int; pushes data:string or Nil.
  - OP_SOCK_CLOSE: Close socket; pops fd:int; pushes 1/0.
- Serial (TTY):
  - OP_SERIAL_OPEN: Open serial port; pops baud:int, path:string; pushes fd:int or -1.
  - OP_SERIAL_CONFIG: Configure port; pops flow_ctrl, stop_bits, parity, data_bits, fd; pushes 1/0.
  - OP_SERIAL_SEND: Send bytes; pops data:string, fd:int; pushes bytesSent:int or -1.
  - OP_SERIAL_RECV: Receive bytes; pops max:int, fd:int; pushes data:string or Nil.
  - OP_SERIAL_CLOSE: Close; pops fd:int; pushes 1/0.

## Curl (HTTP)

- OP_CURL_GET: HTTP GET; pops url:string; pushes body:string or Nil.
- OP_CURL_POST: HTTP POST; pops body:string, url:string; pushes response:string or Nil.
- OP_CURL_DOWNLOAD: Download URL to file; pops path:string, url:string; pushes 1/0.

## OpenSSL (optional)

- OP_OPENSSL_MD5: Compute MD5 digest and return lowercase hex string; pops data:string; pushes md5:string.
- OP_OPENSSL_SHA256: Compute SHA‑256 digest and return lowercase hex string; pops data:string; pushes sha256:string.
- OP_OPENSSL_SHA512: Compute SHA‑512 digest and return lowercase hex string; pops data:string; pushes sha512:string.
  - Requires building with `-DFUN_WITH_OPENSSL=ON`. When disabled, these opcodes still exist but return an empty string to match other optional extensions’ fallback behavior.

## PCRE2 (Regex)

- OP_PCRE2_TEST: Test pattern; pops flags:int, text:string, pattern:string; pushes 1/0.
- OP_PCRE2_MATCH: Match pattern; pops flags:int, text:string, pattern:string; pushes array/map with groups or 0.
- OP_PCRE2_FINDALL: Find all matches; pops flags:int, text:string, pattern:string; pushes array of matches.

## PC/SC (Smart cards)

- OP_PCSC_ESTABLISH: Establish context; pushes ctx handle (>0) or 0.
- OP_PCSC_LIST_READERS: List readers; pops scope/id; pushes array of strings.
- OP_PCSC_CONNECT: Connect to reader; pops reader:string, ctx; pushes handle or 0.
- OP_PCSC_TRANSMIT: Send APDU; pops apdu:array<byte>/string, handle; pushes response bytes or map incl. SW.
- OP_PCSC_DISCONNECT: Disconnect; pops handle; pushes 1/0.
- OP_PCSC_RELEASE: Release context; pops scope/id; pushes 1/0.

## Notcurses (Terminal UI)

- OP_NC_INIT: Initialize notcurses; pushes handle or 0.
- OP_NC_SHUTDOWN: Shutdown; no args; pushes 1/0.
- OP_NC_CLEAR: Clear screen; pushes 1/0.
- OP_NC_DRAW_TEXT: Draw text at (x,y); pops text, x, y; pushes 1/0.
- OP_NC_GETCH: Get key with timeout; pops timeout_ms:int; pushes int key or -1.

## SQLite-compatible lib (libSQL)

- See “libSQL” section above; identical opcode set with different backend.

## TK (Tcl/Tk UI)

- OP_TK_EVAL: Evaluate Tcl code; pops text:string; pushes result string or error.
- OP_TK_LABEL: Create/update label; pops text, id; pushes 1/0.
- OP_TK_BUTTON: Create/update button; pops text, id; pushes 1/0.
- OP_TK_PACK: Pack widget; pops id; pushes 1/0.
- OP_TK_BIND: Bind event; pops command, event, id; pushes 1/0.
- OP_TK_WM_TITLE: Set window title; pops title; pushes 1/0.
- OP_TK_LOOP: Enter main event loop; no args; blocks until exit.
- OP_TK_RESULT: Retrieve last Tcl result; pushes string.

## Miscellaneous

- OP_KEYS / OP_VALUES: Map utilities (see Maps).
- Additional opcodes may exist for modules under src/vm that are stubbed or platform-dependent. Refer to the corresponding C file for precise semantics and edge cases.

## How to explore

- Each opcode implementation lives in its own file and begins with a comment documenting its behavior and stack contract. Grep for “case OP_” or open files under src/vm/<module>/*.c to see details and error handling.
