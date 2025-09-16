/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "value.h"
#include "vm.h"

static char *read_file_all(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[n] = '\0';
    if (out_len) *out_len = n;
    return buf;
}

static void skip_ws(const char *src, size_t len, size_t *pos) {
    while (*pos < len) {
        char c = src[*pos];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { (*pos)++; continue; }
        break;
    }
}

static void skip_line(const char *src, size_t len, size_t *pos) {
    while (*pos < len && src[*pos] != '\n') (*pos)++;
    if (*pos < len && src[*pos] == '\n') (*pos)++;
}

static void skip_comments(const char *src, size_t len, size_t *pos) {
    for (;;) {
        skip_ws(src, len, pos);
        if (*pos + 1 < len && src[*pos] == '/' && src[*pos + 1] == '/') {
            *pos += 2;
            skip_line(src, len, pos);
            continue;
        }
        if (*pos + 1 < len && src[*pos] == '/' && src[*pos + 1] == '*') {
            *pos += 2;
            while (*pos + 1 < len && !(src[*pos] == '*' && src[*pos + 1] == '/')) (*pos)++;
            if (*pos + 1 < len) *pos += 2;
            continue;
        }
        break;
    }
}

static int starts_with(const char *src, size_t len, size_t pos, const char *kw) {
    size_t klen = strlen(kw);
    if (pos + klen > len) return 0;
    return strncmp(src + pos, kw, klen) == 0;
}

static void skip_shebang_if_present(const char *src, size_t len, size_t *pos) {
    if (*pos == 0 && starts_with(src, len, *pos, "#!")) {
        skip_line(src, len, pos);
    }
}

static void skip_identifier(const char *src, size_t len, size_t *pos) {
    size_t p = *pos;
    if (p < len && (isalpha((unsigned char)src[p]) || src[p] == '_')) {
        p++;
        while (p < len && (isalnum((unsigned char)src[p]) || src[p] == '_')) p++;
    }
    *pos = p;
}

static int consume_char(const char *src, size_t len, size_t *pos, char expected) {
    skip_ws(src, len, pos);
    if (*pos < len && src[*pos] == expected) { (*pos)++; return 1; }
    return 0;
}

static char *parse_string_literal_any_quote(const char *src, size_t len, size_t *pos) {
    skip_ws(src, len, pos);
    if (*pos >= len) return NULL;
    char quote = src[*pos];
    if (quote != '"' && quote != '\'') return NULL;
    (*pos)++; // skip opening quote
    size_t cap = 64, out_len = 0;
    char *out = (char*)malloc(cap);
    if (!out) return NULL;
    while (*pos < len) {
        char c = src[*pos];
        if (c == quote) { (*pos)++; break; }
        if (c == '\\') {
            (*pos)++;
            if (*pos >= len) break;
            char e = src[*pos];
            switch (e) {
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                case '\\': c = '\\'; break;
                case '"': c = '"'; break;
                case '\'': c = '\''; break;
                default: c = e; break;
            }
        }
        if (out_len + 1 >= cap) {
            cap *= 2;
            char *tmp = (char*)realloc(out, cap);
            if (!tmp) { free(out); return NULL; }
            out = tmp;
        }
        out[out_len++] = c;
        (*pos)++;
    }
    if (out_len + 1 >= cap) {
        char *tmp = (char*)realloc(out, cap + 1);
        if (!tmp) { free(out); return NULL; }
        out = tmp;
    }
    out[out_len] = '\0';
    return out;
}

/* === Helpers for identifiers, numbers, booleans, and globals === */

static void skip_spaces(const char *src, size_t len, size_t *pos) {
    while (*pos < len) {
        char c = src[*pos];
        if (c == ' ' || c == '\t' || c == '\r') { (*pos)++; continue; }
        break;
    }
}

static int read_identifier_into(const char *src, size_t len, size_t *pos, char **out_name) {
    size_t p = *pos;
    if (p < len && (isalpha((unsigned char)src[p]) || src[p] == '_')) {
        size_t start = p;
        p++;
        while (p < len && (isalnum((unsigned char)src[p]) || src[p] == '_')) p++;
        size_t n = p - start;
        char *name = (char*)malloc(n + 1);
        if (!name) return 0;
        memcpy(name, src + start, n);
        name[n] = '\0';
        *pos = p;
        *out_name = name;
        return 1;
    }
    return 0;
}

static int64_t parse_int_literal_value(const char *src, size_t len, size_t *pos, int *ok) {
    size_t p = *pos;
    skip_spaces(src, len, &p);
    int sign = 1;
    if (p < len && (src[p] == '+' || src[p] == '-')) {
        if (src[p] == '-') sign = -1;
        p++;
    }
    if (p >= len || !isdigit((unsigned char)src[p])) { *ok = 0; return 0; }
    int64_t val = 0;
    while (p < len && isdigit((unsigned char)src[p])) {
        val = val * 10 + (src[p] - '0');
        p++;
    }
    *pos = p;
    *ok = 1;
    return sign * val;
}
