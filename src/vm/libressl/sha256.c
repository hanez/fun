/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

/**
 * LibreSSL SHA-256 builtin
 */
case OP_LIBRESSL_SHA256: {
    Value vdata = pop_value(vm);
    char *s = value_to_string_alloc(&vdata);
    free_value(vdata);
    if (!s) { push_value(vm, make_string("")); break; }
    char *hex = fun_libressl_sha256_hex((const unsigned char*)s, strlen(s));
    free(s);
    if (!hex) { push_value(vm, make_string("")); break; }
    Value out = make_string(hex);
    free(hex);
    push_value(vm, out);
    break;
}
