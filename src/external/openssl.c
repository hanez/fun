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

 /*
 * OpenSSL integration helpers (MD5, RIPEMD-160, SHA-256, SHA-512)
 */

#ifdef FUN_WITH_OPENSSL
#include <openssl/evp.h>
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
    const EVP_MD *md = EVP_md5();
    if (!md) return NULL;
    int dlen = EVP_MD_get_size(md);
    if (dlen <= 0) return NULL;
    unsigned char *digest = (unsigned char*)malloc((size_t)dlen);
    if (!digest) return NULL;
    unsigned int out_len = 0;
    int ok;
    if (len == 0) {
        // EVP_Digest handles zero-length fine as well
        ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
    } else {
        ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
    }
    if (ok != 1 || (int)out_len != dlen) { free(digest); return NULL; }
    char *hex = (char*)malloc((size_t)dlen * 2 + 1);
    if (!hex) { free(digest); return NULL; }
    for (int i = 0; i < dlen; ++i) {
        hex[2*i]   = hexdig[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = hexdig[digest[i] & 0xF];
    }
    hex[dlen * 2] = '\0';
    free(digest);
    return hex;
#else
    char *hex = (char*)malloc(1);
    if (hex) hex[0] = '\0';
    return hex;
#endif
}

/* Compute SHA-256 hex of input buffer; returns malloc'ed C string (lowercase hex).
 * Fallback when OpenSSL disabled: empty string. */
static char *fun_openssl_sha256_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
    const EVP_MD *md = EVP_sha256();
    if (!md) return NULL;
    int dlen = EVP_MD_get_size(md);
    if (dlen <= 0) return NULL;
    unsigned char *digest = (unsigned char*)malloc((size_t)dlen);
    if (!digest) return NULL;
    unsigned int out_len = 0;
    int ok;
    if (len == 0) {
        ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
    } else {
        ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
    }
    if (ok != 1 || (int)out_len != dlen) { free(digest); return NULL; }
    char *hex = (char*)malloc((size_t)dlen * 2 + 1);
    if (!hex) { free(digest); return NULL; }
    for (int i = 0; i < dlen; ++i) {
        hex[2*i]   = hexdig[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = hexdig[digest[i] & 0xF];
    }
    hex[dlen * 2] = '\0';
    free(digest);
    return hex;
#else
    char *hex = (char*)malloc(1);
    if (hex) hex[0] = '\0';
    return hex;
#endif
}

/* Compute SHA-512 hex of input buffer; returns malloc'ed C string (lowercase hex).
 * Fallback when OpenSSL disabled: empty string. */
static char *fun_openssl_sha512_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
    const EVP_MD *md = EVP_sha512();
    if (!md) return NULL;
    int dlen = EVP_MD_get_size(md);
    if (dlen <= 0) return NULL;
    unsigned char *digest = (unsigned char*)malloc((size_t)dlen);
    if (!digest) return NULL;
    unsigned int out_len = 0;
    int ok;
    if (len == 0) {
        ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
    } else {
        ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
    }
    if (ok != 1 || (int)out_len != dlen) { free(digest); return NULL; }
    char *hex = (char*)malloc((size_t)dlen * 2 + 1);
    if (!hex) { free(digest); return NULL; }
    for (int i = 0; i < dlen; ++i) {
        hex[2*i]   = hexdig[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = hexdig[digest[i] & 0xF];
    }
    hex[dlen * 2] = '\0';
    free(digest);
    return hex;
#else
    char *hex = (char*)malloc(1);
    if (hex) hex[0] = '\0';
    return hex;
#endif
}

/* Compute RIPEMD-160 hex of input buffer; returns malloc'ed C string (lowercase hex).
 * Note: On OpenSSL 3.x, RIPEMD160 may require the legacy provider; if unavailable,
 * EVP_ripemd160() can return NULL. In that case we return NULL and the VM opcode
 * will fall back to empty string behavior. When OpenSSL is disabled, return empty string. */
static char *fun_openssl_ripemd160_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
    const EVP_MD *md = EVP_ripemd160();
    if (!md) return NULL;
    int dlen = EVP_MD_get_size(md);
    if (dlen <= 0) return NULL;
    unsigned char *digest = (unsigned char*)malloc((size_t)dlen);
    if (!digest) return NULL;
    unsigned int out_len = 0;
    int ok;
    if (len == 0) {
        ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
    } else {
        ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
    }
    if (ok != 1 || (int)out_len != dlen) { free(digest); return NULL; }
    char *hex = (char*)malloc((size_t)dlen * 2 + 1);
    if (!hex) { free(digest); return NULL; }
    for (int i = 0; i < dlen; ++i) {
        hex[2*i]   = hexdig[(digest[i] >> 4) & 0xF];
        hex[2*i+1] = hexdig[digest[i] & 0xF];
    }
    hex[dlen * 2] = '\0';
    free(digest);
    return hex;
#else
    char *hex = (char*)malloc(1);
    if (hex) hex[0] = '\0';
    return hex;
#endif
}
