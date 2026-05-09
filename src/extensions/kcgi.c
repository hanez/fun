/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file kcgi.c
 * @brief kcgi helpers for Fun VM KCGI-related opcodes (conditional build).
 */

#ifdef FUN_WITH_KCGI

#include <kcgi.h>
#include <stdlib.h>
#include <string.h>

/* Thread-local request handle for the current CGI invocation */
#ifdef _WIN32
static __declspec(thread) struct kreq *g_kcgi_req = NULL;
#else
static __thread struct kreq *g_kcgi_req = NULL;
#endif

static Value kcgi_fields_to_map(const struct kreq *r) {
  Value m = make_map_empty();
  if (!r) return m;
  for (size_t i = 0; i < r->fieldsz; i++) {
    const char *k = r->fields[i].key ? r->fields[i].key : "";
    const char *v = r->fields[i].val ? r->fields[i].val : "";
    map_set(&m, k, make_string(v));
  }
  return m;
}

static Value kreq_to_fun(const struct kreq *r) {
  Value out = make_map_empty();
  if (!r) return out;
  /* Basic request info */
  map_set(&out, "host",   make_string(r->host ? r->host : ""));
  map_set(&out, "port",   make_int((int64_t)r->port));
  map_set(&out, "path",   make_string(r->path ? r->path : ""));
  map_set(&out, "suffix", make_string(r->suffix ? r->suffix : ""));

  Value fields = kcgi_fields_to_map(r);
  map_set(&out, "fields", fields);
  return out;
}

/* Lifecycle helpers used by VM opcodes */
static int kcgi_parse_request(struct kreq **out) {
  static const struct kvalid keys[] = { { NULL, NULL } }; /* accept all */
  struct kreq *r = (struct kreq *)calloc(1, sizeof(*r));
  if (!r) return 0;
  enum kcgi_err er = khttp_parse(r, keys, 0, NULL, 0, 0);
  if (er != KCGI_OK) { free(r); return 0; }
  *out = r;
  return 1;
}

static void kcgi_free_request(struct kreq *r) {
  if (!r) return;
  khttp_free(r);
  free(r);
}

static int kcgi_reply_start(int code, const char *ctype) {
  if (!g_kcgi_req) return 0;
  /* Emit Content-Type header; Status defaults to 200 if not set */
  if (ctype && *ctype) {
    if (khttp_head(g_kcgi_req, "Content-Type", "%s", ctype) != KCGI_OK)
      return 0;
  }
  if (khttp_body(g_kcgi_req) != KCGI_OK)
    return 0;
  return 1;
}

static int kcgi_write_str(const char *s) {
  if (!g_kcgi_req) return 0;
  if (!s) s = "";
  size_t n = strlen(s);
  return khttp_write(g_kcgi_req, s, n) == KCGI_OK;
}

#endif /* FUN_WITH_KCGI */
