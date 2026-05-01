/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file echo.c
 * @brief Implements the OP_ECHO opcode for printing without a trailing newline.
 *
 * This snippet is included into the VM dispatch loop and handles OP_ECHO.
 * It pops the top value from the stack and appends it to the VM's output buffer
 * but marks the entry as partial so that subsequent OP_PRINT may continue the
 * same line.
 *
 * Stack contract:
 * - Pops: value (any)
 * - Pushes: (none)
 */

case OP_ECHO: {
  Value v = pop_value(vm);
  Value snap = deep_copy_value(&v);
  free_value(v);
  if (vm->output_count < OUTPUT_SIZE) {
    int idx = vm->output_count;
    vm->output[idx] = snap;
    vm->output_is_partial[idx] = 1; // ECHO does not end the line
    vm->output_count++;
  } else {
    free_value(snap);
    fprintf(stderr, "Runtime error: output buffer overflow\n");
    exit(1);
  }
  break;
}
