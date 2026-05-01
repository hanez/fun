/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file md5.c
 * @brief Implements OP_OPENSSL_MD5 to compute an MD5 hash in hexadecimal.
 *
 * Behavior:
 * - Pops one value from the VM stack and converts it to a byte sequence by
 *   obtaining its string representation using value_to_string_alloc().
 * - Computes the MD5 digest of the resulting bytes via fun_openssl_md5_hex()
 *   and pushes the lowercase hexadecimal string back onto the stack.
 * - On allocation or hashing failure, pushes an empty string ("").
 *
 * Notes:
 * - Non-string inputs are accepted; they are stringified first.
 * - This snippet is included by vm.c and executed when OP_OPENSSL_MD5 is
 *   dispatched.
 *
 * Errors:
 * - This opcode does not terminate the VM. Failures result in an empty
 *   string being pushed.
 */

case OP_OPENSSL_MD5: {
  Value vdata = pop_value(vm);
  char *s = value_to_string_alloc(&vdata);
  free_value(vdata);
  if (!s) {
    push_value(vm, make_string(""));
    break;
  }
  char *hex = fun_openssl_md5_hex((const unsigned char *)s, strlen(s));
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
