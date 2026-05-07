/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file openssl.c
 * @brief OpenSSL-based hashing helpers used by crypto-related VM opcodes.
 *
 * This module centralizes small, concrete helpers around the OpenSSL EVP
 * message-digest API so that VM opcodes under src/vm/crypto/*.c can remain
 * minimal and focus on VM stack marshalling. Keeping the algorithm-specific
 * logic in src/extensions/ mirrors other extensions (PCRE2, SQLite, XML2,
 * JSON, INI) and improves maintainability.
 *
 * Build-time feature flag:
 * - All code in this file is compiled only when FUN_WITH_OPENSSL is enabled.
 *   When disabled, the helpers provide safe fallbacks that allocate and return
 *   an empty string (""), preserving the VM's expectations around string
 *   ownership while signaling the absence of cryptographic support.
 *
 * Algorithms covered:
 * - MD5
 * - SHA-256
 * - SHA-512
 * - RIPEMD-160 (may be unavailable on some OpenSSL builds; see notes below)
 *
 * Ownership and memory model:
 * - Each helper returns a newly allocated, NUL-terminated lowercase hex string
 *   that the caller owns and must free() when no longer needed.
 * - On allocation failure or when an algorithm/provider is unavailable, NULL
 *   is returned (except in disabled builds where an allocated empty string is
 *   returned). Callers should check for NULL before use.
 *
 * Zero-length input:
 * - The EVP_Digest() API supports computing a digest for zero-length inputs.
 *   Passing len==0 is valid; data may be NULL in that case. Passing data==NULL
 *   with len>0 returns NULL to indicate misuse.
 *
 * Error handling:
 * - Any failure to acquire an EVP_MD, an unexpected digest length, or memory
 *   allocation failure results in a NULL return (again, except in the disabled
 *   build where an allocated empty string is returned to keep the shape).
 *
 * OpenSSL 3.x provider note:
 * - RIPEMD-160 is part of the legacy provider in OpenSSL 3.x and may not be
 *   available unless the provider is enabled at runtime/compile time. In such
 *   environments EVP_ripemd160() can return NULL and this module will surface
 *   that as a NULL return value to the caller.
 *
 * Thread-safety:
 * - The helpers are stateless and thread-safe as long as the underlying
 *   OpenSSL library initialization/finalization follows OpenSSL's guidelines
 *   for multithreaded use. No shared global state is kept here.
 */

/*
 * OpenSSL integration helpers (MD5, RIPEMD-160, SHA-256, SHA-512)
 */

#ifdef FUN_WITH_OPENSSL
#include <openssl/evp.h>
/**
 * @brief Forward declaration for EVP_MD_get_size on older headers.
 *
 * Some older OpenSSL headers might not declare EVP_MD_get_size even though the
 * symbol is available in libcrypto. Guarded declaration keeps compilation
 * working across versions while always calling the same function.
 */
#ifndef EVP_MD_get_size
int EVP_MD_get_size(const EVP_MD *md);
#endif
#endif
#include <stdlib.h>

/**
 * @brief Compute MD5 and return it as a lowercase hexadecimal string.
 *
 * Behavior and edge cases:
 * - Accepts zero-length input (len==0); in this case data may be NULL.
 * - Returns a string of length 32 characters (2 hex chars per 16-byte digest)
 *   plus the NUL terminator, on success.
 * - Returns NULL if the algorithm is unavailable or on allocation failure.
 * - When FUN_WITH_OPENSSL is disabled, returns an allocated empty string "".
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len  Number of input bytes.
 * @return char* Newly-allocated NUL-terminated hex string on success; NULL on
 *               failure (except disabled builds return an allocated empty
 *               string). The caller must free() the returned buffer.
 */
static char *fun_openssl_md5_hex(const unsigned char *data, size_t len) {
  static const char hexdig[] = "0123456789abcdef";
  if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
  const EVP_MD *md = EVP_md5();
  if (!md) return NULL;
  int dlen = EVP_MD_get_size(md);
  if (dlen <= 0) return NULL;
  unsigned char *digest = (unsigned char *)malloc((size_t)dlen);
  if (!digest) return NULL;
  unsigned int out_len = 0;
  int ok;
  if (len == 0) {
    // EVP_Digest handles zero-length fine as well
    ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
  } else {
    ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
  }
  if (ok != 1 || (int)out_len != dlen) {
    free(digest);
    return NULL;
  }
  char *hex = (char *)malloc((size_t)dlen * 2 + 1);
  if (!hex) {
    free(digest);
    return NULL;
  }
  for (int i = 0; i < dlen; ++i) {
    hex[2 * i] = hexdig[(digest[i] >> 4) & 0xF];
    hex[2 * i + 1] = hexdig[digest[i] & 0xF];
  }
  hex[dlen * 2] = '\0';
  free(digest);
  return hex;
#else
  char *hex = (char *)malloc(1);
  if (hex) hex[0] = '\0';
  return hex;
#endif
}

/**
 * @brief Compute SHA-256 and return it as a lowercase hexadecimal string.
 *
 * Details:
 * - Output length is 64 hex characters (2 per 32-byte digest) plus NUL.
 * - data may be NULL if len==0; otherwise must be non-NULL.
 * - Returns NULL on failure; in disabled builds returns an allocated "".
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len  Number of input bytes.
 * @return char* Newly-allocated hex string on success; NULL on failure (except
 *               disabled builds return an allocated empty string). The caller
 *               must free() the buffer.
 */
static char *fun_openssl_sha256_hex(const unsigned char *data, size_t len) {
  static const char hexdig[] = "0123456789abcdef";
  if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
  const EVP_MD *md = EVP_sha256();
  if (!md) return NULL;
  int dlen = EVP_MD_get_size(md);
  if (dlen <= 0) return NULL;
  unsigned char *digest = (unsigned char *)malloc((size_t)dlen);
  if (!digest) return NULL;
  unsigned int out_len = 0;
  int ok;
  if (len == 0) {
    ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
  } else {
    ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
  }
  if (ok != 1 || (int)out_len != dlen) {
    free(digest);
    return NULL;
  }
  char *hex = (char *)malloc((size_t)dlen * 2 + 1);
  if (!hex) {
    free(digest);
    return NULL;
  }
  for (int i = 0; i < dlen; ++i) {
    hex[2 * i] = hexdig[(digest[i] >> 4) & 0xF];
    hex[2 * i + 1] = hexdig[digest[i] & 0xF];
  }
  hex[dlen * 2] = '\0';
  free(digest);
  return hex;
#else
  char *hex = (char *)malloc(1);
  if (hex) hex[0] = '\0';
  return hex;
#endif
}

/**
 * @brief Compute SHA-512 and return it as a lowercase hexadecimal string.
 *
 * Details:
 * - Output length is 128 hex characters (2 per 64-byte digest) plus NUL.
 * - data may be NULL if len==0; otherwise must be non-NULL.
 * - Returns NULL on failure; in disabled builds returns an allocated "".
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len  Number of input bytes.
 * @return char* Newly-allocated hex string on success; NULL on failure (except
 *               disabled builds return an allocated empty string). The caller
 *               must free() the buffer.
 */
static char *fun_openssl_sha512_hex(const unsigned char *data, size_t len) {
  static const char hexdig[] = "0123456789abcdef";
  if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
  const EVP_MD *md = EVP_sha512();
  if (!md) return NULL;
  int dlen = EVP_MD_get_size(md);
  if (dlen <= 0) return NULL;
  unsigned char *digest = (unsigned char *)malloc((size_t)dlen);
  if (!digest) return NULL;
  unsigned int out_len = 0;
  int ok;
  if (len == 0) {
    ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
  } else {
    ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
  }
  if (ok != 1 || (int)out_len != dlen) {
    free(digest);
    return NULL;
  }
  char *hex = (char *)malloc((size_t)dlen * 2 + 1);
  if (!hex) {
    free(digest);
    return NULL;
  }
  for (int i = 0; i < dlen; ++i) {
    hex[2 * i] = hexdig[(digest[i] >> 4) & 0xF];
    hex[2 * i + 1] = hexdig[digest[i] & 0xF];
  }
  hex[dlen * 2] = '\0';
  free(digest);
  return hex;
#else
  char *hex = (char *)malloc(1);
  if (hex) hex[0] = '\0';
  return hex;
#endif
}

/**
 * @brief Compute RIPEMD-160 and return it as a lowercase hexadecimal string.
 *
 * Availability and details:
 * - On OpenSSL 3.x, RIPEMD-160 typically resides in the legacy provider and
 *   may be unavailable unless explicitly enabled; EVP_ripemd160() can return
 *   NULL in that case and this helper returns NULL.
 * - Output length is 40 hex characters (2 per 20-byte digest) plus NUL.
 * - data may be NULL if len==0; otherwise must be non-NULL.
 * - In disabled builds, returns an allocated empty string.
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len  Number of input bytes.
 * @return char* Newly-allocated hex string on success; NULL if the algorithm
 *               is unavailable or on error (except disabled builds return an
 *               allocated empty string). Caller must free() the buffer.
 */
static char *fun_openssl_ripemd160_hex(const unsigned char *data, size_t len) {
  static const char hexdig[] = "0123456789abcdef";
  if (!data && len != 0) return NULL;
#ifdef FUN_WITH_OPENSSL
  const EVP_MD *md = EVP_ripemd160();
  if (!md) return NULL;
  int dlen = EVP_MD_get_size(md);
  if (dlen <= 0) return NULL;
  unsigned char *digest = (unsigned char *)malloc((size_t)dlen);
  if (!digest) return NULL;
  unsigned int out_len = 0;
  int ok;
  if (len == 0) {
    ok = EVP_Digest(NULL, 0, digest, &out_len, md, NULL);
  } else {
    ok = EVP_Digest(data, len, digest, &out_len, md, NULL);
  }
  if (ok != 1 || (int)out_len != dlen) {
    free(digest);
    return NULL;
  }
  char *hex = (char *)malloc((size_t)dlen * 2 + 1);
  if (!hex) {
    free(digest);
    return NULL;
  }
  for (int i = 0; i < dlen; ++i) {
    hex[2 * i] = hexdig[(digest[i] >> 4) & 0xF];
    hex[2 * i + 1] = hexdig[digest[i] & 0xF];
  }
  hex[dlen * 2] = '\0';
  free(digest);
  return hex;
#else
  char *hex = (char *)malloc(1);
  if (hex) hex[0] = '\0';
  return hex;
#endif
}
