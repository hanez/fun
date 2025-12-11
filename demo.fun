#!/usr/bin/env fun

/*
 * Interactive demo runner for all examples in ./examples
 * - Asks y/n before running each feature demo
 * - Executes each example as a subprocess
 */

#include <io/console.fun>

fun pick_fun_bin()
  // Prefer an explicit FUN_BIN override; otherwise rely on PATH
  b = env("FUN_BIN")
  if b != ""
    return b
  return "fun"

fun run_example(bin, path)
  // Ensure examples can locate stdlib when run from repo root.
  // We execute via the shell so env assignment + redirection works.
  cmd = join(["sh -c '\nFUN_LIB_DIR=./lib ", bin, " ", path, " 2>&1\n'"], "")
  print("-- output begin --")
  code = system(cmd)
  print("-- output end --")
  print(join(["exit code: ", to_string(code)], ""))
  return code

fun main()
  c = Console()
  bin = pick_fun_bin()

  print("=== Fun language feature showcase (interactive) ===")
  print(join(["Using interpreter: ", bin], ""))
  print("Tip: set FUN_BIN=/path/to/fun to override. Stdlib is passed via FUN_LIB_DIR=./lib\n")

  // List of example scripts. Keep paths relative to repo root where this demo resides.
  // If you add/remove examples, update this list.
  files = [
    "examples/arrays.fun",
    "examples/arrays_advanced.fun",
    "examples/arrays_iter.fun",
    "examples/boolean_decl.fun",
    "examples/booleans.fun",
    "examples/builtins_conversions.fun",
    "examples/builtins_extended.fun",
    "examples/builtins_maps_and_more.fun",
    "examples/byte_for_demo.fun",
    "examples/byte_overflow_try_catch.fun",
    "examples/cast_demo.fun",
    "examples/class_constructor.fun",
    "examples/classes_demo.fun",
    "examples/crc32_example.fun",
    "examples/crc32c_example.fun",
    "examples/curl_download.fun",
    "examples/curl_get_json.fun",
    "examples/curl_post.fun",
    "examples/datetime_basic.fun",
    "examples/datetime_extended.fun",
    "examples/datetime_timer.fun",
    "examples/debug_reporting.fun",
    "examples/echo_example.fun",
    "examples/exit_example.fun",
    "examples/expressions_test.fun",
    "examples/fail.fun",
    "examples/file_io.fun",
    "examples/file_print_for_file_line_by_line.fun",
    "examples/floats.fun",
    "examples/for_range_test.fun",
    "examples/functions_test.fun",
    "examples/have_fun.fun",
    "examples/have_fun_function.fun",
    "examples/if_else_test.fun",
    "examples/include_lib.fun",
    "examples/include_local.fun",
    "examples/include_local_util.fun",
    "examples/include_namespace.fun",
    "examples/inheritance_demo.fun",
    "examples/ini_class_demo.fun",
    "examples/ini_complex.fun",
    "examples/ini_demo.fun",
    "examples/ini_subsections.fun",
    "examples/input_example.fun",
    "examples/json_showcase.fun",
    "examples/libsql_example.fun",
    "examples/loops_break_continue.fun",
    "examples/md5_demo.fun",
    "examples/namespaced_mod.fun",
    "examples/nested_loops.fun",
    "examples/objects_basic.fun",
    "examples/objects_more.fun",
    "examples/os_env.fun",
    "examples/pcre2_opcodes.fun",
    "examples/pcre2_showcase.fun",
    "examples/pcsc_example.fun",
    "examples/process_example.fun",
    "examples/regex_demo.fun",
    "examples/regex_procedural.fun",
    "examples/repl_on_error.fun",
    "examples/sha1_demo.fun",
    "examples/sha256_demo.fun",
    "examples/sha256_str_demo.fun",
    "examples/sha384_example.fun",
    "examples/sha512_demo.fun",
    "examples/sha512_str_demo.fun",
    "examples/short_circuit_test.fun",
    "examples/signed_ints.fun",
    "examples/sqlite_example.fun",
    "examples/stdlib_showcase.fun",
    "examples/strings_test.fun",
    "examples/tcp_http_get.fun",
    "examples/tcp_http_get_class.fun",
    "examples/thread_class_example.fun",
    "examples/threads_demo.fun",
    "examples/tk_hello.fun",
    "examples/try_catch_finally.fun",
    "examples/try_catch_with_error.fun",
    "examples/typeof.fun",
    "examples/typeof_features.fun",
    "examples/type_safety.fun",
    "examples/type_safety_fails.fun",
    "examples/types_integers.fun",
    "examples/types_overview.fun",
    "examples/uint_types.fun",
    "examples/unix_socket_echo.fun",
    "examples/while_test.fun",
    "examples/xml_access_catalog.fun",
    "examples/xml_access_employees.fun",
    "examples/xml_access_ns.fun",
    "examples/xml_class_example.fun",
    "examples/xml_minimal.fun"
  ]

  failures = []

  for f in files
    q = join(["Run ", f, "?"], "")
    if c.ask_yes_no(q)
      print(join(["=== Running: ", f, " ==="], ""))
      code = run_example(bin, f)
      if code != 0
        failures.push(f)
      print("")
    else
      print(join(["Skipped: ", f], ""))

  if len(failures) == 0
    print("All selected examples completed successfully.")
  else
    print("Some selected examples failed:")
    for ff in failures
      print(join([" - ", ff], ""))

main()
