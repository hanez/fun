/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file ripemd160.c
 * @brief Implements OP_OPENSSL_RIPEMD160 to compute a RIPEMD-160 hash in hex.
 *
 * Behavior:
 * - Pops one value from the VM stack and converts it to bytes by first
 *   producing its string representation with value_to_string_alloc().
 * - Computes the RIPEMD-160 digest via fun_openssl_ripemd160_hex() and
 *   pushes the lowercase hexadecimal string.
 * - On allocation or hashing failure, pushes an empty string ("").
 *
 * Notes:
 * - Accepts any value type; non-strings are stringified first.
 * - Included by vm.c and executed for OP_OPENSSL_RIPEMD160.
 *
 * Errors:
 * - Does not abort the VM; failures result in an empty string.
 */

case OP_OPENSSL_RIPEMD160: {
  Value vdata = pop_value(vm);
  char *s = value_to_string_alloc(&vdata);
  free_value(vdata);
  if (!s) {
    push_value(vm, make_string(""));
    break;
  }
  char *hex = fun_openssl_ripemd160_hex((const unsigned char *)s, strlen(s));
  free(s);
  if (!hex) {
    push_value(vm, make_string(""));
    break;
  }
  Value out = make_string(hex);
  free(hex);
  push_value(vm, out);
  break;
}
