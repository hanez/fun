#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-04-21
 */

// Interactive showcase script for Fun examples.
// Converted from Bash to Fun.

#include <io/console.fun>
#include <cli.fun>
#include <strings.fun>

// Set FUN_LIB_DIR to ensure stdlib is found
// We try to use existing environment or default to ./lib
lib_dir = env("FUN_LIB_DIR")
if (len(to_string(lib_dir)) == 0)
    lib_dir = "./lib"

// Discovery logic for the interpreter
fun find_interpreter()
    bin = env("FUN_BIN")
    if (len(to_string(bin)) > 0)
        return bin

    // Check common build locations
    // We use 'system' with 'test -f' as we don't have a direct 'is_file' yet
    if (system("test -f ./build/fun") == 0)
        return "./build/fun"
    if (system("test -f ./build_debug/fun") == 0)
        return "./build_debug/fun"
    if (system("test -f ./build_release/fun") == 0)
        return "./build_release/fun"

    // Check PATH
    if (system("command -v fun >/dev/null 2>&1") == 0)
        return "fun"

    return ""

interpreter = find_interpreter()
if (len(interpreter) == 0)
    print("Error: fun interpreter not found. Please build it or set FUN_BIN.")
    exit(1)

print("Using interpreter: " + interpreter)
print("Searching for examples in ./examples...")

// Recursive file discovery
// Since we don't have a robust recursive os_list_dir yet, 
// and the original used 'find', we'll use 'find' via system to get the list.
fun get_examples()
    // We use 'find' to get all .fun files and sort them.
    // We redirect to a temporary file as we don't have a 'proc_capture' that returns an array easily
    // Actually, os_list_dir uses 'ls -1'. We can use a similar approach or just call find.

    cmd = "find examples -name \"*.fun\" | sort > ./tmp/examples_list.txt"
    system(cmd)

    list_str = read_file("./tmp/examples_list.txt")
    // We don't have a built-in 'split' that handles newlines easily in all versions? 
    // Wait, lib/strings.fun might have it.

    lines = str_split(list_str, "\n")

    // Filter out empty lines
    examples = []
    i = 0
    n = len(lines)
    while (i < n)
        line = str_trim(lines[i])
        if (len(line) > 0)
            push(examples, line)
        i = i + 1
    return examples

examples = get_examples()
if (len(examples) == 0)
    print("No examples found in ./examples")
    exit(0)

failed_examples = []
console = Console()

i = 0
n = len(examples)
while (i < n)
    example = examples[i]
    print("--------------------------------------------------------------------------------")
    print("Example: " + example)

    // ask_yes_no returns 1 for yes, 0 for no
    print("Do you want to run this example? [y/N]")
    ans = str_to_lower(str_trim(input("")))
    if (ans == "y" || ans == "yes")
        print("Running: " + interpreter + " " + example)

        // Prepare command with environment variable
        // FUN_LIB_DIR must be passed to the child process
        cmd = "FUN_LIB_DIR=\"" + lib_dir + "\" " + interpreter + " " + example
        exit_code = system(cmd)

        print("Exit code: " + to_string(exit_code))
        if (exit_code != 0)
            push(failed_examples, example + " (exit code: " + to_string(exit_code) + ")")
    else
        print("Skipping.")

    i = i + 1

print("--------------------------------------------------------------------------------")
print("Done.")

if (len(failed_examples) > 0)
    print("The following examples failed:")
    i = 0
    while (i < len(failed_examples))
        print("  - " + failed_examples[i])
        i = i + 1
    exit(1)

exit(0)
