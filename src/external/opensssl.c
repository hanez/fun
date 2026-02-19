/*
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-02-19
 */

 /*
 * OpenSSL integration helpers (currently MD5 only)
 */

#ifdef FUN_WITH_OPENSSL
#  include <openssl/md5.h>
#endif
#include <stdlib.h>

/* Compute MD5 hex of input buffer; returns malloc'ed C string (lowercase hex).
 * Caller must free(). Returns NULL on failure. When OpenSSL is disabled,
 * returns an allocated empty string ("") to keep behavior consistent with
 * other optional extensions. */
static char *fun_openssl_md5_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;
    if (MD5_Init(&ctx) != 1) return NULL;
    if (len && MD5_Update(&ctx, data, len) != 1) return NULL;
    if (MD5_Final(digest, &ctx) != 1) return NULL;
    char *hex = (char*)malloc(MD5_DIGEST_LENGTH * 2 + 1);
    if (!hex) return NULL;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        hex[2*i]   = hexdig[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = hexdig[digest[i] & 0xF];
    }
    hex[MD5_DIGEST_LENGTH * 2] = '\0';
    return hex;
#else
    char *hex = (char*)malloc(1);
    if (hex) hex[0] = '\0';
    return hex;
#endif
}
