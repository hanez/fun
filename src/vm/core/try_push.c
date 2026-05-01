/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file try_push.c
 * @brief Implements the OP_TRY_PUSH opcode to begin a try/catch region.
 *
 * This file handles the OP_TRY_PUSH instruction, which marks the start of a
 * try/catch region in the current frame by pushing the index of the TRY
 * instruction onto a small per-frame stack. The actual catch target IP is
 * stored in the TRY instruction's operand and may be patched later by the
 * compiler/linker.
 *
 * Behavior:
 * - Pushes the index of this TRY instruction (f->ip - 1) onto f->try_stack.
 * - Does not modify the value stack.
 *
 * Errors:
 * - If the try depth exceeds the size of f->try_stack, prints a runtime error
 *   and terminates the process.
 */

case OP_TRY_PUSH: {
  /* push index of this TRY instruction; handler ip is in its operand (may be patched later) */
  if (f->try_sp >= (int)(sizeof(f->try_stack) / sizeof(f->try_stack[0])) - 1) {
    fprintf(stderr, "Runtime error: try depth exceeded\n");
    exit(1);
  }
  f->try_stack[++f->try_sp] = f->ip - 1; /* index of TRY_PUSH instruction */
  break;
}
