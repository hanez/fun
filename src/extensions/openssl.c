/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file openssl.c
 * @brief OpenSSL-based hashing helpers used by crypto-related opcodes.
 *
 * Provides thin wrappers around OpenSSL EVP routines to compute common
 * digests (MD5, RIPEMD-160, SHA-256, SHA-512). When FUN_WITH_OPENSSL is
 * disabled, these helpers fall back to returning empty strings to keep the
 * VM behavior consistent.
 */

/*
 * OpenSSL integration helpers (MD5, RIPEMD-160, SHA-256, SHA-512)
 */

#ifdef FUN_WITH_OPENSSL
#include <openssl/evp.h>
/* Always use EVP_MD_get_size; if headers don't declare it, provide a
 * forward declaration to allow linking against OpenSSL libcrypto. */
#ifndef EVP_MD_get_size
int EVP_MD_get_size(const EVP_MD *md);
#endif
#endif
#include <stdlib.h>

/**
 * @brief Compute MD5 digest and return it as a lowercase hex string.
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len Number of input bytes.
 * @return Newly-allocated NUL-terminated hex string on success; NULL on
 *         failure. When FUN_WITH_OPENSSL is disabled, returns an allocated
 *         empty string to keep behavior consistent.
 * @note The caller owns the returned buffer and must free() it.
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
 * @brief Compute SHA-256 digest and return it as a lowercase hex string.
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len Number of input bytes.
 * @return Newly-allocated hex string on success; NULL on failure. When
 *         FUN_WITH_OPENSSL is disabled, returns an allocated empty string.
 * @note The caller must free() the returned buffer.
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
 * @brief Compute SHA-512 digest and return it as a lowercase hex string.
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len Number of input bytes.
 * @return Newly-allocated hex string on success; NULL on failure. When
 *         FUN_WITH_OPENSSL is disabled, returns an allocated empty string.
 * @note The caller must free() the returned buffer.
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
 * @brief Compute RIPEMD-160 digest and return it as a lowercase hex string.
 *
 * On some OpenSSL builds (e.g., 3.x without legacy provider), RIPEMD-160 may
 * be unavailable and EVP_ripemd160() can return NULL.
 *
 * @param data Pointer to input bytes (may be NULL if len==0).
 * @param len Number of input bytes.
 * @return Newly-allocated hex string on success; NULL if the digest is
 *         unavailable or another error occurs. When FUN_WITH_OPENSSL is
 *         disabled, returns an allocated empty string.
 * @note The caller must free() the returned buffer.
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
