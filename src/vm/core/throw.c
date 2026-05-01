/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file throw.c
 * @brief Implements the OP_THROW opcode for raising exceptions in the VM.
 *
 * This file handles the OP_THROW instruction, which raises an exception. If a
 * matching TRY handler exists in the current frame (pushed via OP_TRY_PUSH),
 * control flow jumps to the handler location and the error value is made
 * available to the catch block via the stack. If no handler is present in the
 * current frame, the error is printed and the VM terminates execution by
 * clearing the frame stack.
 *
 * Behavior:
 * - Pops the error `Value` from the stack.
 * - If the current frame has a pending TRY handler (f->try_sp >= 0):
 *   - Retrieves the handler target IP from the TRY instruction's operand.
 *   - Pushes the error back on the stack for the catch block to consume.
 *   - Sets the instruction pointer (IP) to the handler target.
 * - Otherwise (no handler):
 *   - Prints the error in a human-readable form.
 *   - Frees the error value and clears all frames (vm->fp = -1) to stop the VM.
 *
 * Errors:
 * - None explicitly thrown here; if unhandled, the VM stops after printing the
 *   error message.
 */

case OP_THROW: {
  Value err = pop_value(vm);
  /* if there is a handler in this frame, jump to it and push err for catch */
  if (f->try_sp >= 0) {
    int try_idx = f->try_stack[f->try_sp--];
    int target = f->fn->instructions[try_idx].operand;
    /* push error for catch block */
    push_value(vm, err); /* transfer ownership to stack */
    f->ip = target;
    break;
  }
  /* Unhandled: print error and terminate */
  char *s = value_to_string_alloc(&err);
  if (s) {
    fprintf(stdout, "%s\n", s);
    free(s);
  } else {
    fprintf(stdout, "<error>\n");
  }
  free_value(err);
  /* clear frames to stop execution */
  vm->fp = -1;
  break;
}
