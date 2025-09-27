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

/* === Include preprocessor ===
 * Supports:
 *   - #include "path"  (resolved relative to current working directory)
 *   - #include <path>  (resolved under /usr/lib/fun/)
 * Also accepts 'include' without '#', at the start of a line (after spaces/tabs).
 * Directives are recognized only when not inside strings or block comments.
 */

static void *xrealloc(void *ptr, size_t newcap) {
    void *np = realloc(ptr, newcap);
    return np;
}

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} StrBuf;

static void sb_init(StrBuf *sb) {
    sb->buf = (char*)malloc(256);
    sb->cap = sb->buf ? 256 : 0;
    sb->len = 0;
    if (sb->buf) sb->buf[0] = '\0';
}

static void sb_reserve(StrBuf *sb, size_t need) {
    if (need <= sb->cap) return;
    size_t nc = sb->cap ? sb->cap : 256;
    while (nc < need) nc *= 2;
    char *nb = (char*)xrealloc(sb->buf, nc);
    if (!nb) return;
    sb->buf = nb;
    sb->cap = nc;
}

static void sb_append_n(StrBuf *sb, const char *s, size_t n) {
    if (n == 0) return;
    sb_reserve(sb, sb->len + n + 1);
    if (!sb->buf) return;
    memcpy(sb->buf + sb->len, s, n);
    sb->len += n;
    sb->buf[sb->len] = '\0';
}

static void sb_append(StrBuf *sb, const char *s) {
    sb_append_n(sb, s, strlen(s));
}

static void sb_append_ch(StrBuf *sb, char c) {
    sb_reserve(sb, sb->len + 2);
    if (!sb->buf) return;
    sb->buf[sb->len++] = c;
    sb->buf[sb->len] = '\0';
}

static char *preprocess_includes_internal(const char *src, int depth) {
    if (!src) return NULL;
    if (depth > 64) {
        fprintf(stderr, "Include error: include nesting too deep\n");
        return strdup("");
    }

    const char *LIB_DIR = "/usr/lib/fun/";
    size_t len = strlen(src);
    StrBuf out;
    sb_init(&out);
    int in_line = 0, in_block = 0, in_sq = 0, in_dq = 0, esc = 0;
    int bol = 1; /* beginning of line */

    for (size_t i = 0; i < len; ) {
        char c = src[i];

        /* Detect include directive at BOL, outside comments/strings */
        if (bol && !in_block && !in_sq && !in_dq) {
            size_t j = i;
            /* skip leading spaces/tabs */
            while (j < len && (src[j] == ' ' || src[j] == '\t')) j++;
            size_t k = j;
            if (k < len && src[k] == '#') k++;
            const char *kw = "include";
            size_t kwlen = 7;
            if (k + kwlen <= len && strncmp(src + k, kw, kwlen) == 0) {
                k += kwlen;
                /* next must be space/tab or delimiter */
                while (k < len && (src[k] == ' ' || src[k] == '\t')) k++;
                if (k < len && (src[k] == '"' || src[k] == '<')) {
                    char opener = src[k];
                    char closer = (opener == '"') ? '"' : '>';
                    k++;
                    size_t path_start = k;
                    while (k < len && src[k] != closer) k++;
                    if (k < len && src[k] == closer) {
                        size_t path_len = k - path_start;
                        char *path = (char*)malloc(path_len + 1);
                        if (path) {
                            memcpy(path, src + path_start, path_len);
                            path[path_len] = '\0';

                            /* advance to end of line */
                            k++;
                            while (k < len && src[k] != '\n') k++;
                            if (k < len && src[k] == '\n') k++;

                            /* resolve file path */
                            char resolved[1024];
                            if (opener == '<') {
                                snprintf(resolved, sizeof(resolved), "%s%s", LIB_DIR, path);
                            } else {
                                snprintf(resolved, sizeof(resolved), "%s", path);
                            }
                            free(path);

                            /* read and recursively expand */
                            size_t inc_len = 0;
                            char *inc = read_file_all(resolved, &inc_len);
                            if (!inc) {
                                fprintf(stderr, "Include error: cannot read '%s'\n", resolved);
                                sb_append(&out, "// include error: cannot read ");
                                sb_append(&out, resolved);
                                sb_append(&out, "\n");
                            } else {
                                char *exp = preprocess_includes_internal(inc, depth + 1);
                                free(inc);
                                if (exp) {
                                    sb_append(&out, exp);
                                    /* ensure included chunk ends with newline to preserve line structure */
                                    if (out.len == 0 || out.buf[out.len - 1] != '\n') sb_append_ch(&out, '\n');
                                    free(exp);
                                }
                            }

                            /* move input pointer to start of next line */
                            i = k;
                            bol = 1;
                            continue;
                        }
                    }
                }
            }
        }

        /* normal stateful copy with comment/string tracking */
        if (in_line) {
            sb_append_ch(&out, c);
            if (c == '\n') { in_line = 0; bol = 1; } else { bol = 0; }
            i++;
            continue;
        }
        if (in_block) {
            sb_append_ch(&out, c);
            if (c == '*' && (i + 1) < len && src[i + 1] == '/') {
                sb_append_ch(&out, '/');
                i += 2;
                bol = 0;
                in_block = 0;
                continue;
            }
            bol = (c == '\n') ? 1 : 0;
            i++;
            continue;
        }
        if (in_sq) {
            sb_append_ch(&out, c);
            if (!esc && c == '\\') { esc = 1; i++; bol = 0; continue; }
            if (!esc && c == '\'') { in_sq = 0; }
            esc = 0;
            bol = (c == '\n') ? 1 : 0;
            i++;
            continue;
        }
        if (in_dq) {
            sb_append_ch(&out, c);
            if (!esc && c == '\\') { esc = 1; i++; bol = 0; continue; }
            if (!esc && c == '"') { in_dq = 0; }
            esc = 0;
            bol = (c == '\n') ? 1 : 0;
            i++;
            continue;
        }

        /* outside any special state */
        if (c == '/' && (i + 1) < len && src[i + 1] == '/') {
            sb_append_ch(&out, '/');
            sb_append_ch(&out, '/');
            i += 2;
            in_line = 1;
            bol = 0;
            continue;
        }
        if (c == '/' && (i + 1) < len && src[i + 1] == '*') {
            sb_append_ch(&out, '/');
            sb_append_ch(&out, '*');
            i += 2;
            in_block = 1;
            bol = 0;
            continue;
        }
        if (c == '\'') {
            sb_append_ch(&out, c);
            in_sq = 1;
            bol = 0;
            i++;
            continue;
        }
        if (c == '"') {
            sb_append_ch(&out, c);
            in_dq = 1;
            bol = 0;
            i++;
            continue;
        }

        sb_append_ch(&out, c);
        bol = (c == '\n') ? 1 : 0;
        i++;
    }

    if (!out.buf) return strdup("");
    /* ensure NUL-terminated */
    if (out.cap == out.len) sb_reserve(&out, out.len + 1);
    if (out.buf) out.buf[out.len] = '\0';
    return out.buf;
}

static char *preprocess_includes(const char *src) {
    return preprocess_includes_internal(src, 0);
}
