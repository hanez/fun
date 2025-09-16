/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file read_file.c
 * @brief Implements the OP_READ_FILE opcode for reading file contents in the VM.
 *
 * This file handles the OP_READ_FILE instruction, which reads the contents of a file
 * and pushes the result as a string onto the stack.
 *
 * Behavior:
 * - Pops the file path from the stack.
 * - Reads the file contents.
 * - Pushes the contents as a string onto the stack.
 *
 * Error Handling:
 * - Exits with an error if the file path is invalid or the file cannot be read.
 *
 * Example:
 * // Bytecode: OP_READ_FILE
 * // Stack before: ["file.txt"]
 * // Stack after: ["file contents"]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_READ_FILE: {
    Value path = pop_value(vm);
    if (path.type != VAL_STRING) { fprintf(stderr, "READ_FILE expects string\n"); exit(1); }
    const char *p = path.s ? path.s : "";
    FILE *f = fopen(p, "rb");
    if (!f) { free_value(path); push_value(vm, make_string("")); break; }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); free_value(path); push_value(vm, make_string("")); break; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); free_value(path); push_value(vm, make_string("")); break; }
    rewind(f);
    char *buf = (char*)malloc((size_t)sz + 1);
    size_t n = buf ? fread(buf, 1, (size_t)sz, f) : 0;
    fclose(f);
    if (!buf) { free_value(path); push_value(vm, make_string("")); break; }
    buf[n] = '\0';
    Value out = make_string(buf);
    free(buf);
    free_value(path);
    push_value(vm, out);
    break;
}
