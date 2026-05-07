/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file curl.c
 * @brief libcurl helpers and buffers used by HTTP-related VM opcodes.
 *
 * This module centralizes small, concrete libcurl utilities used by VM
 * opcodes under src/vm/http/*.c (or similar). The opcodes themselves perform
 * VM stack marshalling and call these helpers for I/O glue. Keeping the
 * concrete logic here mirrors other extensions (e.g., PCRE2, SQLite) and
 * allows the opcode code to remain minimal.
 *
 * Build-time feature flag:
 * - The code in this file is compiled only when FUN_WITH_CURL is enabled.
 *   When disabled, curl-related opcodes should be compiled with no-op
 *   fallbacks in their respective files.
 *
 * Buffering and ownership model:
 * - FunCurlBuf is a small growable buffer intended for use with libcurl's
 *   CURLOPT_WRITEFUNCTION callback. The buffer expands with realloc() as data
 *   arrives and is maintained NUL-terminated for convenience when the content
 *   is treated as a C-string. The caller is responsible for allocating and
 *   freeing the FunCurlBuf fields (i.e., initialize { .d=NULL, .n=0 } before
 *   first use and free(b.d) afterwards).
 *
 * Callbacks provided:
 * - fun_curl_write_cb(): appends incoming data to a FunCurlBuf, keeping it
 *   NUL-terminated. Returns the number of bytes handled (sz*nm) on success or
 *   0 on allocation failure to signal an error to libcurl.
 * - fun_curl_file_write_cb(): writes incoming data directly to a FILE* passed
 *   via CURLOPT_WRITEDATA. Returns the number of elements written as per
 *   fwrite().
 *
 * Error handling:
 * - The write-to-buffer callback returns 0 on realloc failure, causing libcurl
 *   to abort the transfer and report CURLE_WRITE_ERROR. The file writer relies
 *   on fwrite()'s return value; callers should check the CURLcode after
 *   performing the transfer for final status.
 *
 * Thread-safety:
 * - These helpers themselves are stateless and thread-safe as long as each
 *   CURL easy handle and associated buffers/FILE* are not shared concurrently
 *   across threads without external synchronization.
 */

/* Ensure libcurl headers and helpers are defined at file scope (not inside vm_run) */
#ifdef FUN_WITH_CURL
#include <curl/curl.h>

/**
 * @brief Simple growable buffer for libcurl write callbacks.
 *
 * The buffer grows via realloc as more data arrives. The content is kept
 * NUL-terminated for convenience so it can be treated as a C-string.
 *
 * Usage pattern:
 * - Initialize: FunCurlBuf b = { .d = NULL, .n = 0 };
 * - Set CURLOPT_WRITEFUNCTION = fun_curl_write_cb and CURLOPT_WRITEDATA = &b
 * - After curl_easy_perform(), b.d points to the collected data (length b.n).
 * - Free with free(b.d) when done.
 */
typedef struct {
  char *d;   /**< Data pointer (NUL-terminated). */
  size_t n;  /**< Number of bytes stored (excluding NUL). */
} FunCurlBuf;

/**
 * @brief libcurl write callback that appends data to a FunCurlBuf.
 *
 * Reallocates the destination buffer as needed and keeps it NUL-terminated.
 * The buffer must be initialized by the caller with { .d=NULL, .n=0 } prior to
 * first use.
 *
 * @param ptr Pointer to incoming data block provided by libcurl.
 * @param sz Size of each data element (as provided by libcurl).
 * @param nm Number of elements in this block (as provided by libcurl).
 * @param ud User data; must be a FunCurlBuf* (passed via CURLOPT_WRITEDATA).
 * @return Number of bytes actually handled (sz*nm) on success; 0 on failure to
 *         signal an error to libcurl (which will abort with CURLE_WRITE_ERROR).
 */
static size_t fun_curl_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  size_t add = sz * nm;
  FunCurlBuf *b = (FunCurlBuf *)ud;
  char *p = (char *)realloc(b->d, b->n + add + 1);
  if (!p) return 0;
  memcpy(p + b->n, ptr, add);
  b->d = p;
  b->n += add;
  b->d[b->n] = '\0';
  return add;
}

/**
 * @brief libcurl write callback that writes directly to a FILE*.
 *
 * Suitable for large downloads or when incremental flushing to disk is
 * desired. The FILE* should be opened in binary mode (e.g., "wb").
 *
 * @param ptr Pointer to incoming data.
 * @param sz Size of each element.
 * @param nm Number of elements.
 * @param ud User data; must be a FILE* opened for writing in binary mode
 *           (passed via CURLOPT_WRITEDATA).
 * @return Number of elements written (as returned by fwrite). Returning a
 *         value smaller than nm will signal an error to libcurl.
 */
static size_t fun_curl_file_write_cb(void *ptr, size_t sz, size_t nm, void *ud) {
  FILE *f = (FILE *)ud;
  return fwrite(ptr, sz, nm, f);
}
#endif
