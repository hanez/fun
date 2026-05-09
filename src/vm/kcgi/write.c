/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file write.c
 * @brief VM opcode snippet for writing HTTP response body via kcgi.
 *
 * This snippet is included by vm.c and implements the OP_KCGI_WRITE
 * instruction. It pops one value, converts it to a UTF-8 string, and
 * writes it to the HTTP response using kcgi_write_str(). It then pushes
 * 1 on success or 0 on failure.
 *
 * Build gating: compiled only when FUN_WITH_KCGI is enabled. When disabled,
 * the opcode consumes its argument (if any) and pushes 0.
 *
 * Stack effect (with FUN_WITH_KCGI):
 * - Pops: text (any; converted to string; empty string if NULL)
 * - Pushes: int 1 on success, int 0 on failure
 *
 * Notes:
 * - Ownership: A temporary C string is allocated for conversion and freed
 *   after the write. The pushed Value follows normal VM ownership rules.
 */

/* KCGI_WRITE */
case OP_KCGI_WRITE: {
#ifdef FUN_WITH_KCGI
  Value vs = pop_value(vm);
  char *s = value_to_string_alloc(&vs);
  free_value(vs);
  int ok = kcgi_write_str(s ? s : "");
  if (s) free(s);
  push_value(vm, make_int(ok ? 1 : 0));
#else
  Value drop = pop_value(vm); free_value(drop);
  push_value(vm, make_int(0));
#endif
  break;
}
