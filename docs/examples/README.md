# Examples catalog and how to run them

This page is a practical catalog of the example areas that ship with the Fun language. It explains what you’ll find in each folder under ./examples/, how to run the scripts, and where deeper walkthroughs live.

If you’re building/running from the repository without installing, set FUN_LIB_DIR to the local ./lib directory so examples can locate the stdlib:

- export FUN_LIB_DIR="./lib"
- Then run an example: ./build_debug/fun examples/basics/hello_world.fun
  - Or, if installed: fun examples/basics/hello_world.fun

Notes
- Most scripts are self‑documented with a short header comment at the top. Open them to see exact behavior and expected output.
- Some examples depend on optional extensions (cURL, PCRE2, SQLite, Notcurses, Tk, PC/SC, OpenSSL/LibreSSL). See docs/external/ for enablement and availability.
- Networking examples often bind to 127.0.0.1 on high ports; read the file header for the exact port.

Deep‑dives

## Top‑level examples (./examples)

Quick micro demos covering language features and tiny utilities.

Run pattern:
- fun examples/<file>.fun

Highlights (selection):

- [builtins_extended.fun](./builtins_extended.md) — tour of core built‑ins beyond the basics
- [byte_for_demo.fun](./byte_for_demo.md) — iterating bytes with for
- [byte_overflow_try_catch.fun](./byte_overflow_try_catch.md) — error handling on overflow
- [cast_demo.fun](./cast_demo.md) — conversions and casting helpers
- [class_constructor.fun](./class_constructor.md) — basic constructor usage and init patterns
- [class_test.fun](./class_test.md) — small class feature checks
- [classes_demo.fun](classes_demo.md) — class basics and usage
- [class_without_object.fun](./class_without_object.md) — class features without an instance helper
- [cli_argv_dump.fun](./cli_argv_dump.md) — prints argv/argc handling
- [conversions_showcase.fun](./conversions_showcase.md) — numbers, strings, bytes conversions
- [cpp_add.fun](./cpp_add.md) — calling into the optional C++ extension (if enabled)
- [datetime_basic.fun](./datetime_basic.md) — minimal date/time helpers
- [datetime_extended.fun](./datetime_extended.md) — richer date/time operations
- [datetime_timer.fun](datetime_timer.md) — time/date helpers
- [echo_example.fun](./echo_example.md) — simple echo of input/args
- [env_all.fun](./env_all.md) — enumerate environment variables
- [os_env.fun](./os_env.md) — reading environment variables
- [error_handling.fun](./error_handling.md) — try/catch and error objects
- [try_catch_finally.fun](try_catch_finally.md) — error patterns
- [expressions_test.fun](./expressions_test.md) — precedence and grouping
- [features.fun](./features.md) — grab bag of language features
- [file_print_for_file_line_by_line.fun](./file_print_for_file_line_by_line.md) — iterate file content
- [floats.fun](./floats.md) — float ops and formatting
- [for_range_test.fun](./for_range_test.md) — ranges and loops
- [functions_test.fun](./functions_test.md) — functions, closures, returns
- [have_fun.fun](./have_fun.md) — playful starter demo
- [have_fun_function.fun](have_fun_function.md) — playful demos
- [hex_example.fun](./hex_example.md) — hex encode/decode
- [if_else_test.fun](./if_else_test.md) — conditional branching examples
- [nested_loops.fun](./nested_loops.md) — loops inside loops
- [loops_break_continue.fun](loops_break_continue.md) — control flow
- [include_local.fun](./include_local.md) — include a local file/module
- [include_lib.fun](./include_lib.md) — include from the stdlib path
- [include_local_util.fun](include_local_util.md) — include mechanics
- [inheritance_demo.fun](./inheritance_demo.md) — simple class inheritance
- [maps.fun](./maps.md), [match.fun](match.md) — data structures and pattern matching
- [namespaced_mod.fun](./namespaced_mod.md) — namespaced includes with as
- [objects_basic.fun](./objects_basic.md) — simple objects and methods
- [objects_more.fun](./objects_more.md) — OO basics
- [process_example.fun](./process_example.md) — spawn and capture subprocess output
- [progress.fun](./progress.md) — progress bar helper
- [progress_inline.fun](./progress_inline.md) — simple progress displays
- [random_demo.fun](./random_demo.md) — random helpers overview
- [random_number_example.fun](random_number_example.md) — RNG usage
- [regex_demo.fun](./regex_demo.md) — regex basics (PCRE2 when enabled)
- [regex_procedural.fun](./regex_procedural.md) — regex helpers (PCRE2 when enabled)
- [rust_hello.fun](./rust_hello.md), rust_hello_args*.fun — Rust opcodes (if FUN_WITH_RUST)
- [serial_demo.fun](./serial_demo.md) — serial port usage
- [test_serial.fun](./test_serial.md) — serial port (when available)
- [short_circuit_test.fun](./short_circuit_test.md) — boolean evaluation order
- [signed_ints.fun](./signed_ints.md) — signed integers overview
- [uint_types.fun](./uint_types.md) — unsigned integers overview
- [types_integers.fun](./types_integers.md) — integer families
- [stdlib_showcase.fun](./stdlib_showcase.md) — sampler of common stdlib modules
- [strings_test.fun](./strings_test.md) — string helpers and edge cases
- [tcp_http_get.fun](./tcp_http_get.md) — simple HTTP GET over raw TCP
- [tcp_http_get_class.fun](./tcp_http_get_class.md) — manual HTTP client over sockets
- [test_bits.fun](./test_bits.md) — /rol/shl/xor/dec_to_hex/hex_to_dec bitwise utilities
- [thread_class_example.fun](./thread_class_example.md) — define and run a thread class
- [threads_demo.fun](./threads_demo.md) — threading building blocks
- [typeof.fun](./typeof.md) — type inspection
- [typeof_features.fun](./typeof_features.md) — type inspection of declared integer subtypes and runtime categories
- [type_safety.fun](./type_safety.md) — static/dynamic type checks
- [type_safety_fails.fun](./type_safety_fails.md) — static/dynamic type checks
- [types_overview.fun](./types_overview.md) — language types tour
- [unix_socket_echo.fun](./unix_socket_echo.md) — local domain socket echo demo
- [version.fun](./version.md) — print VM/version info
- [while_test.fun](./while_test.md) — simple while loop example

Tip: If a file name is listed above but not present on your build, it may depend on an extension you did not enable.


## Algorithms (./examples/algos)
- deduplicate.fun — removing duplicates from arrays/maps
- sort_and_search.fun — sorting and lookup patterns
- stack_queue.fun — basic stack and queue implementation

## Arrays (./examples/arrays)
- arrays.fun — create, index, slice; typical idioms
- arrays_iter.fun — iteration and enumeration
- arrays_advanced.fun — copying, filtering, transformations

## Basics (./examples/basics)
- boolean_decl.fun, booleans.fun — boolean values and operators
- builtins_conversions.fun — core built‑ins, type conversions
- collections.fun — arrays, maps, nested structures
- fibonacci.fun, fizzbuzz.fun — classic exercises
- hello_world.fun — the canonical first program

## CLI (./examples/cli)
- args_parse.fun — arguments parsing patterns for small CLIs

## Compose (./examples/compose)
Compositional patterns, small abstractions to combine behavior.

## Crypto (./examples/crypto)
Hashing and cryptographic helpers. Availability depends on whether OpenSSL/LibreSSL is enabled.
- openssl_md5.fun — MD5 via OpenSSL (if -DFUN_WITH_OPENSSL)
- libressl_md5.fun — MD5 via LibreSSL (if -DFUN_WITH_LIBRESSL)
- aes256.fun and hash samples if present on your build

## Data (./examples/data)
Static assets for example servers; not meant to be run directly.
- htdocs/ — files used by HTTP server examples (index.html, hello.fun, info.fun, form_post.fun, counter.fun, redirect.fun, json_like_api.fun)

## Error handling and diagnostics (./examples/error)
- debug_reporting.fun — enabling debug output and reading traces
- exit_example.fun — exit codes
- fail.fun — triggering and observing failures
- repl_on_error.fun — dropping into REPL on error
- rust_vm_access.fun — Rust opcode errors (if enabled)
- test_indent.fun — parser/indentation corner cases
- try_catch_with_error.fun — capturing error objects

## Extra integrations (./examples/extra)
These require optional external libraries. See docs/external/.
- curl_* — cURL HTTP client examples (download, GET JSON, POST)
- ini_* — parsing INI files (simple to complex)
- json_showcase.fun — JSON helpers
- libsql_example.fun — libSQL client usage
- notcurses_* — rich TUI demos (if Notcurses enabled)
- pcre2_* — PCRE2 regex engine demos
- pcsc*.fun — smart card access via PC/SC
- sqlite_example.fun — SQLite usage
- tcp_echo_server*.fun — basic TCP echo server
- tk_* — Tcl/Tk GUI examples
- xml_* — XML parsing with libxml2

## Interactive (./examples/interactive)
- console_prompt.fun — simple prompt loop
- input_example.fun — reading user input
- input_hidden_example.fun, input_hidden_pam_auth.fun — hidden input/passwords

## IO (./examples/io)
- async_http_client.fun — non‑blocking HTTP client
- await_http_client.fun — non‑blocking HTTP client using lib/async/scheduler.fun (await-style helpers)
- csv_reader.fun — parse CSV files
- file_io.fun, read_write_file.fun — file operations
- word_count.fun — classic WC example

## Math (./examples/math)
Small, focused math helpers and demonstrations:
- math_ceil, math_floor, math_round, math_trunc, math_sign
- math_cos, math_sin, math_tan
- math_sqrt, math_isqrt
- math_exp_log
- math_fmin_fmax
- math_gcd_lcm

## Networking (./examples/net)
Servers and socket utilities. See docs/examples/net/httpserver.md for the HTTP family.
- HTTP servers: [httpserver.md](./net/httpserver.md) — end‑to‑end walkthrough of all HTTP server variants in examples/net/
- http_static_server.fun — minimal static server over sockets
- http_server.fun — blocking static/CGI dispatcher using lib/net/http_server.fun
- http_server_cgi.fun — blocking server with CGI via lib/net/http_cgi_server.fun
- http_server_cgi_lib.fun — blocking server leveraging net/cgi.fun helpers
- http_mt_server.fun — thread‑per‑connection static server
- http_mt_server_cgi.fun — thread‑per‑connection with CGI support

## Patterns (./examples/patterns)
Small idioms and reusable patterns.
- assert_like.fun — assert‑style checks via language constructs

## Snippets (./examples/snippets)
Miscellaneous one‑off code snippets demonstrating particular opcodes or tricks.

## SQLite daemon (./examples/sqlited)
- Files related to running a small SQLite‑backed service (see source for details)

## Strings (./examples/strings)
- base64_demo.fun — base64 encode/decode using encoding/base64
- split_join_trim.fun — string splitting and trimming
- templating_min.fun — bare‑bones templating
- urlencode_decode.fun — percent‑encoding helpers

## Broken (./examples/broken)
Historical or intentionally broken examples kept for reference/regression.
- notcurses_* — experiments around Notcurses
- ripemd160* — legacy or experimental hash routines

### Running examples reliably

Prefer the local build when running from the repository root:
- ./build_debug/fun <path/to/example.fun>
- or: ./build_release/fun <path/to/example.fun>

Set up env if needed:
- export FUN_LIB_DIR="./lib" # to find stdlib
- export FUN_EXEC="./build_debug/fun" # used by CGI examples
- export FUN_HTDOCS="./examples/data/htdocs" # override docroot

When optional extensions are not enabled, their examples will not run; reconfigure the build with the required -D flags shown in docs/external/.
