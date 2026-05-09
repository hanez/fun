/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file reply_start.c
 * @brief VM opcode snippet to start an HTTP reply using kcgi.
 *
 * This snippet is included by vm.c and implements the OP_KCGI_REPLY_START
 * instruction. It pops two values from the stack: the content type and an
 * HTTP status code. Values are converted leniently (int/float/string to code;
 * any value to string for the content type). If the content type is missing
 * or empty, a default of "text/html; charset=utf-8" is used. The opcode then
 * calls kcgi_reply_start(code, content_type) and pushes 1 on success or 0 on
 * failure.
 *
 * Build gating: compiled only when FUN_WITH_KCGI is enabled. When disabled,
 * the opcode consumes both arguments and pushes 0.
 *
 * Stack effect (with FUN_WITH_KCGI):
 * - Pops: content_type (any; converted to string), status_code (int|float|string)
 * - Pushes: int 1 on success, int 0 on failure
 */

/* KCGI_REPLY_START */
case OP_KCGI_REPLY_START: {
#ifdef FUN_WITH_KCGI
  Value vct = pop_value(vm);
  Value vcode = pop_value(vm);
  int code = 200;
  if (vcode.type == VAL_INT) {
    code = (int)vcode.i;
  } else if (vcode.type == VAL_FLOAT) {
    code = (int)vcode.d;
  } else if (vcode.type == VAL_STRING) {
    if (vcode.s) code = atoi(vcode.s);
  }
  char *ct = value_to_string_alloc(&vct);
  free_value(vcode);
  free_value(vct);
  if (!ct || ct[0] == '\0') {
    /* default content type */
    free(ct);
    ct = strdup("text/html; charset=utf-8");
  }
  int ok = kcgi_reply_start(code, ct);
  free(ct);
  push_value(vm, make_int(ok ? 1 : 0));
#else
  Value a = pop_value(vm); free_value(a);
  Value b = pop_value(vm); free_value(b);
  push_value(vm, make_int(0));
#endif
  break;
}
