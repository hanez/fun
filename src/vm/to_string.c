/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file to_string.c
 * @brief Implements the OP_TO_STRING opcode for converting values to strings in the VM.
 *
 * This file handles the OP_TO_STRING instruction, which converts a value of any type
 * into its string representation and pushes the result onto the stack.
 *
 * Behavior:
 * - Pops a value from the stack.
 * - Converts the value to a string based on its type:
 *   - Integers: Convert to decimal string (e.g., 42 → "42")
 *   - Strings: Return a copy of the string
 *   - Arrays: Convert to "[array n=<length>]"
 *   - Maps: Convert to "{map n=<length>}"
 *   - Functions: Convert to "<function@<address>>"
 *   - Nil: Convert to "nil"
 * - Pushes the resulting string onto the stack.
 *
 * Error Handling:
 * - Exits with an error if memory allocation fails during string creation.
 *
 * Example:
 * // Bytecode: OP_TO_STRING
 * // Stack before: [42]
 * // Stack after: ["42"]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_TO_STRING: {
    Value v = pop_value(vm);
    char *s = value_to_string_alloc(&v);
    Value out = make_string(s ? s : "");
    if (s) free(s);
    free_value(v);
    push_value(vm, out);
    break;
}
