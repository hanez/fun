/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file write_file.c
 * @brief Implements the OP_WRITE_FILE opcode for writing to a file in the VM.
 *
 * This file handles the OP_WRITE_FILE instruction, which writes data to a file.
 * The file path and data are popped from the stack, and a success/failure flag is pushed back.
 *
 * Behavior:
 * - Pops the file path and data from the stack.
 * - Writes the data to the file.
 * - Pushes 1 (success) or 0 (failure) onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the file path is invalid or the file cannot be written.
 *
 * Example:
 * // Bytecode: OP_WRITE_FILE
 * // Stack before: ["file.txt", "data"]
 * // Stack after: [1]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_WRITE_FILE: {
    Value data = pop_value(vm);
    Value path = pop_value(vm);
    if (path.type != VAL_STRING || data.type != VAL_STRING) { fprintf(stderr, "WRITE_FILE expects (string, string)\n"); exit(1); }
    const char *p = path.s ? path.s : "";
    FILE *f = fopen(p, "wb");
    int ok = 0;
    if (f) {
        size_t len = data.s ? strlen(data.s) : 0;
        ok = (fwrite(data.s ? data.s : "", 1, len, f) == len);
        fclose(f);
    }
    free_value(path);
    free_value(data);
    push_value(vm, make_int(ok ? 1 : 0));
    break;
}
