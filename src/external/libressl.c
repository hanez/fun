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
 * LibreSSL integration helpers (MD5, RIPEMD-160, SHA-256, SHA-512)
 */

#ifdef FUN_WITH_LIBRESSL
/*
 * When building with LibreSSL, include the LibreSSL-provided headers explicitly.
 * Downstream distros often ship them under /usr/include/libressl/openssl/…
 * and expect the include path to point at /usr/include/libressl so that any
 * nested `#include <openssl/...>` inside these headers resolves to the
 * corresponding LibreSSL headers as well (not the system OpenSSL 3.x ones).
 */
#include <libressl/openssl/evp.h>
/* Compatibility: Prefer EVP_MD_get_size universally. If LibreSSL headers
 * don't declare it, provide a forward declaration so we can link against
 * OpenSSL's libcrypto symbol when that is what CMake found. */
#ifndef EVP_MD_get_size
int EVP_MD_get_size(const EVP_MD *md);
#endif
#endif
#include <stdlib.h>

/* Compute MD5 hex of input buffer; returns malloc'ed C string (lowercase hex). */
static char *fun_libressl_md5_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_LIBRESSL
    const EVP_MD *md = EVP_md5();
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

/* Compute SHA-256 hex of input buffer; returns malloc'ed C string (lowercase hex). */
static char *fun_libressl_sha256_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_LIBRESSL
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

/* Compute SHA-512 hex of input buffer; returns malloc'ed C string (lowercase hex). */
static char *fun_libressl_sha512_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_LIBRESSL
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

/* Compute RIPEMD-160 hex of input buffer; returns malloc'ed C string (lowercase hex). */
static char *fun_libressl_ripemd160_hex(const unsigned char *data, size_t len) {
    static const char hexdig[] = "0123456789abcdef";
    if (!data && len != 0) return NULL;
#ifdef FUN_WITH_LIBRESSL
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
