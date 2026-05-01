/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file sha256.c
 * @brief Implements OP_OPENSSL_SHA256 to compute a SHA-256 hash in hex.
 *
 * Behavior:
 * - Pops one value from the VM stack and converts it to bytes by first
 *   producing its string form with value_to_string_alloc().
 * - Computes the SHA-256 digest via fun_openssl_sha256_hex() and pushes the
 *   lowercase hexadecimal string.
 * - On allocation or hashing failure, pushes an empty string ("").
 *
 * Notes:
 * - Non-string inputs are allowed; values are stringified first.
 * - Snippet is included by vm.c and executed for OP_OPENSSL_SHA256.
 *
 * Errors:
 * - No hard VM error is raised; failures push an empty string.
 */

case OP_OPENSSL_SHA256: {
  Value vdata = pop_value(vm);
  char *s = value_to_string_alloc(&vdata);
  free_value(vdata);
  if (!s) {
    push_value(vm, make_string(""));
    break;
  }
  char *hex = fun_openssl_sha256_hex((const unsigned char *)s, strlen(s));
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
