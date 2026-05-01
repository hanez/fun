/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file parser_utils.c
 * @brief Low-level parsing helpers and include preprocessor for the Fun parser.
 *
 * This module provides character scanners, token helpers, simple string/number
 * literal readers, a lightweight include preprocessor that can expand
 * `include` directives, and mapping utilities to translate expanded line
 * numbers back to original files for diagnostics.
 */
#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Read entire file into a newly allocated buffer.
 *
 * @param path     Path to file.
 * @param out_len  Optional; receives number of bytes read (not including NUL).
 * @return Newly allocated NUL-terminated buffer on success, or NULL on error.
 *         Caller must free() the returned buffer.
 */
static char *read_file_all(const char *path, size_t *out_len) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return NULL;
  }
  long sz = ftell(f);
  if (sz < 0) {
    fclose(f);
    return NULL;
  }
  rewind(f);
  char *buf = (char *)malloc((size_t)sz + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  size_t n = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  buf[n] = '\0';
  if (out_len) *out_len = n;
  return buf;
}

/**
 * @brief Skip spaces, tabs, carriage returns and newlines.
 * @param src Source buffer.
 * @param len Buffer length.
 * @param pos In/out byte offset; advanced past whitespace.
 */
static void skip_ws(const char *src, size_t len, size_t *pos) {
  while (*pos < len) {
    char c = src[*pos];
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      (*pos)++;
      continue;
    }
    break;
  }
}

/**
 * @brief Advance @p pos to the next line, consuming the trailing '\n' if present.
 */
static void skip_line(const char *src, size_t len, size_t *pos) {
  while (*pos < len && src[*pos] != '\n')
    (*pos)++;
  if (*pos < len && src[*pos] == '\n') (*pos)++;
}

/**
 * @brief Skip whitespace, then line and block comments.
 * Continues until the next non-comment, non-whitespace character.
 */
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
      while (*pos + 1 < len && !(src[*pos] == '*' && src[*pos + 1] == '/'))
        (*pos)++;
      if (*pos + 1 < len) *pos += 2;
      continue;
    }
    break;
  }
}

/**
 * @brief Check if src starting at pos begins with kw and fits in len.
 * @return 1 if matches, 0 otherwise.
 */
static int starts_with(const char *src, size_t len, size_t pos, const char *kw) {
  size_t klen = strlen(kw);
  if (pos + klen > len) return 0;
  return strncmp(src + pos, kw, klen) == 0;
}

/**
 * @brief Skip a top-of-file shebang line that starts with "#!" if present.
 */
static void skip_shebang_if_present(const char *src, size_t len, size_t *pos) {
  if (*pos == 0 && starts_with(src, len, *pos, "#!")) {
    skip_line(src, len, pos);
  }
}

/**
 * @brief If an identifier starts at pos, advance pos to its end.
 * Recognizes [A-Za-z_][A-Za-z0-9_]*.
 */
static void skip_identifier(const char *src, size_t len, size_t *pos) {
  size_t p = *pos;
  if (p < len && (isalpha((unsigned char)src[p]) || src[p] == '_')) {
    p++;
    while (p < len && (isalnum((unsigned char)src[p]) || src[p] == '_'))
      p++;
  }
  *pos = p;
}

/**
 * @brief Consume expected character after skipping whitespace.
 * @return 1 if consumed; 0 if not present.
 */
static int consume_char(const char *src, size_t len, size_t *pos, char expected) {
  skip_ws(src, len, pos);
  if (*pos < len && src[*pos] == expected) {
    (*pos)++;
    return 1;
  }
  return 0;
}

/**
 * @brief Parse a single-quoted or double-quoted string literal.
 *
 * Supports common C-style escapes: \n, \r, \t, \\, \", \\' . Returns a newly
 * allocated buffer on success and advances pos. On failure returns NULL and
 * does not modify the source. Caller must free() returned buffer.
 */
static char *parse_string_literal_any_quote(const char *src, size_t len, size_t *pos) {
  skip_ws(src, len, pos);
  if (*pos >= len) return NULL;
  char quote = src[*pos];
  if (quote != '"' && quote != '\'') return NULL;
  (*pos)++; // skip opening quote
  size_t cap = 64, out_len = 0;
  char *out = (char *)malloc(cap);
  if (!out) return NULL;
  while (*pos < len) {
    char c = src[*pos];
    if (c == quote) {
      (*pos)++;
      break;
    }
    if (c == '\\') {
      (*pos)++;
      if (*pos >= len) break;
      char e = src[*pos];
      switch (e) {
      case 'n':
        c = '\n';
        break;
      case 'r':
        c = '\r';
        break;
      case 't':
        c = '\t';
        break;
      case '\\':
        c = '\\';
        break;
      case '"':
        c = '"';
        break;
      case '\'':
        c = '\'';
        break;
      default:
        c = e;
        break;
      }
    }
    if (out_len + 1 >= cap) {
      cap *= 2;
      char *tmp = (char *)realloc(out, cap);
      if (!tmp) {
        free(out);
        return NULL;
      }
      out = tmp;
    }
    out[out_len++] = c;
    (*pos)++;
  }
  if (out_len + 1 >= cap) {
    char *tmp = (char *)realloc(out, cap + 1);
    if (!tmp) {
      free(out);
      return NULL;
    }
    out = tmp;
  }
  out[out_len] = '\0';
  return out;
}

/* === Helpers for identifiers, numbers, booleans, and globals === */

/**
 * @brief Skip only spaces, tabs and carriage returns (not newlines).
 */
static void skip_spaces(const char *src, size_t len, size_t *pos) {
  while (*pos < len) {
    char c = src[*pos];
    if (c == ' ' || c == '\t' || c == '\r') {
      (*pos)++;
      continue;
    }
    break;
  }
}

/**
 * @brief Read an identifier starting at pos and allocate its name.
 * @param out_name Set to malloc'd NUL-terminated name on success.
 * @return 1 on success (pos advanced), 0 otherwise.
 */
static int read_identifier_into(const char *src, size_t len, size_t *pos, char **out_name) {
  size_t p = *pos;
  if (p < len && (isalpha((unsigned char)src[p]) || src[p] == '_')) {
    size_t start = p;
    p++;
    while (p < len && (isalnum((unsigned char)src[p]) || src[p] == '_'))
      p++;
    size_t n = p - start;
    char *name = (char *)malloc(n + 1);
    if (!name) return 0;
    memcpy(name, src + start, n);
    name[n] = '\0';
    *pos = p;
    *out_name = name;
    return 1;
  }
  return 0;
}

/**
 * @brief Parse an integer literal (decimal or 0x-hex) with optional sign.
 * @param ok Set to 1 on success, 0 on failure.
 * @return Parsed value (two's complement cast for negative inputs).
 */
static uint64_t parse_int_literal_value(const char *src, size_t len, size_t *pos, int *ok) {
  size_t p = *pos;
  skip_spaces(src, len, &p);
  int sign = 1;
  if (p < len && (src[p] == '+' || src[p] == '-')) {
    if (src[p] == '-') sign = -1;
    p++;
  }
  if (p >= len) {
    *ok = 0;
    return 0;
  }

  /* Hexadecimal: 0x... or 0X... */
  if ((p + 1) < len && src[p] == '0' && (src[p + 1] == 'x' || src[p + 1] == 'X')) {
    p += 2;
    if (p >= len || !isxdigit((unsigned char)src[p])) {
      *ok = 0;
      return 0;
    }
    uint64_t val = 0;
    while (p < len && isxdigit((unsigned char)src[p])) {
      char c = src[p];
      int d = (c >= '0' && c <= '9')   ? (c - '0')
              : (c >= 'a' && c <= 'f') ? (c - 'a' + 10)
              : (c >= 'A' && c <= 'F') ? (c - 'A' + 10)
                                       : 0;
      val = (val << 4) + (uint64_t)d;
      p++;
    }
    *pos = p;
    *ok = 1;
    return (uint64_t)((int64_t)sign * (int64_t)val);
  }

  /* Decimal fallback */
  if (!isdigit((unsigned char)src[p])) {
    *ok = 0;
    return 0;
  }
  uint64_t val = 0;
  while (p < len && isdigit((unsigned char)src[p])) {
    val = val * 10 + (uint64_t)(src[p] - '0');
    p++;
  }
  *pos = p;
  *ok = 1;
  return (uint64_t)((int64_t)sign * (int64_t)val);
}

/* === Include preprocessor ===
 * Supports:
 *   - #include "path"  (resolved relative to current working directory)
 *   - #include <path>  (resolved under /usr/lib/fun/)
 * Also accepts 'include' without '#', at the start of a line (after spaces/tabs).
 * Directives are recognized only when not inside strings or block comments.
 */

/**
 * @brief Thin wrapper over realloc used by local buffers.
 */
static void *xrealloc(void *ptr, size_t newcap) {
  void *np = realloc(ptr, newcap);
  return np;
}

/**
 * @brief Simple growable string buffer.
 */
typedef struct {
  char *buf;
  size_t len;
  size_t cap;
} StrBuf;

/**
 * @brief Initialize a StrBuf with a small starting capacity.
 */
static void sb_init(StrBuf *sb) {
  sb->buf = (char *)malloc(256);
  sb->cap = sb->buf ? 256 : 0;
  sb->len = 0;
  if (sb->buf) sb->buf[0] = '\0';
}

/**
 * @brief Ensure buffer capacity for at least need bytes (including terminator).
 */
static void sb_reserve(StrBuf *sb, size_t need) {
  if (need <= sb->cap) return;
  size_t nc = sb->cap ? sb->cap : 256;
  while (nc < need)
    nc *= 2;
  char *nb = (char *)xrealloc(sb->buf, nc);
  if (!nb) return;
  sb->buf = nb;
  sb->cap = nc;
}

/**
 * @brief Append n bytes from s to the buffer.
 */
static void sb_append_n(StrBuf *sb, const char *s, size_t n) {
  if (n == 0) return;
  sb_reserve(sb, sb->len + n + 1);
  if (!sb->buf) return;
  memcpy(sb->buf + sb->len, s, n);
  sb->len += n;
  sb->buf[sb->len] = '\0';
}

/**
 * @brief Append a NUL-terminated string to the buffer.
 */
static void sb_append(StrBuf *sb, const char *s) {
  sb_append_n(sb, s, strlen(s));
}

/**
 * @brief Append a single character to the buffer.
 */
static void sb_append_ch(StrBuf *sb, char c) {
  sb_reserve(sb, sb->len + 2);
  if (!sb->buf) return;
  sb->buf[sb->len++] = c;
  sb->buf[sb->len] = '\0';
}

/* ---- Export collection for include-as namespaces ---- */
/**
 * @brief List of exported symbol names discovered at top level.
 */
typedef struct {
  char **names;
  int count;
  int cap;
} NameList;

/**
 * @brief Initialize an empty NameList.
 */
static void nl_init(NameList *nl) {
  nl->names = NULL;
  nl->count = 0;
  nl->cap = 0;
}

/**
 * @brief Add a copy of name to the list (ignores NULL/empty).
 */
static void nl_add(NameList *nl, const char *name) {
  if (!name || !name[0]) return;
  if (nl->count >= nl->cap) {
    int ncap = nl->cap ? nl->cap * 2 : 8;
    char **nn = (char **)realloc(nl->names, (size_t)ncap * sizeof(char *));
    if (!nn) return;
    nl->names = nn;
    nl->cap = ncap;
  }
  nl->names[nl->count++] = strdup(name);
}

/**
 * @brief Free all strings and internal storage in the list.
 */
static void nl_free(NameList *nl) {
  if (!nl) return;
  for (int i = 0; i < nl->count; ++i)
    free(nl->names[i]);
  free(nl->names);
  nl->names = NULL;
  nl->count = nl->cap = 0;
}

/* Collect top-level (indent=0) exported symbols: function and class names.
   Ignores lines inside comments/strings and ignores nested indent. */
/**
 * @brief Collect top-level exported symbols (fun/class) from source text.
 *        Ignores strings and comments. Only lines with zero indentation count.
 */
static void collect_exports_top_level(const char *text, NameList *out) {
  if (!text || !out) return;
  size_t len = strlen(text);
  int in_line = 0, in_block = 0, in_sq = 0, in_dq = 0, esc = 0;
  int bol = 1;
  for (size_t i = 0; i < len;) {
    char c = text[i];

    if (in_line) {
      if (c == '\n') {
        in_line = 0;
        bol = 1;
      } else {
        bol = 0;
      }
      i++;
      continue;
    }
    if (in_block) {
      if (c == '*' && (i + 1) < len && text[i + 1] == '/') {
        i += 2;
        bol = 0;
        in_block = 0;
        continue;
      }
      bol = (c == '\n');
      i++;
      continue;
    }
    if (in_sq) {
      if (!esc && c == '\\') {
        esc = 1;
        i++;
        bol = 0;
        continue;
      }
      if (!esc && c == '\'') {
        in_sq = 0;
      }
      esc = 0;
      bol = (c == '\n');
      i++;
      continue;
    }
    if (in_dq) {
      if (!esc && c == '\\') {
        esc = 1;
        i++;
        bol = 0;
        continue;
      }
      if (!esc && c == '"') {
        in_dq = 0;
      }
      esc = 0;
      bol = (c == '\n');
      i++;
      continue;
    }

    if (c == '/' && (i + 1) < len && text[i + 1] == '/') {
      in_line = 1;
      bol = 0;
      i += 2;
      continue;
    }
    if (c == '/' && (i + 1) < len && text[i + 1] == '*') {
      in_block = 1;
      bol = 0;
      i += 2;
      continue;
    }
    if (c == '\'') {
      in_sq = 1;
      bol = 0;
      i++;
      continue;
    }
    if (c == '"') {
      in_dq = 1;
      bol = 0;
      i++;
      continue;
    }

    if (bol) {
      /* Compute leading spaces to filter out indented constructs */
      size_t j = i;
      int spaces = 0;
      while (j < len && text[j] == ' ') {
        spaces++;
        j++;
      }
      if (j < len && text[j] == '\t') {
        /* tabs not allowed for indentation; treat as not top-level */
        bol = 0;
        i = j + 1;
        continue;
      }
      /* Only consider top-level (indent == 0) */
      if (spaces == 0) {
        /* Check for 'fun ' or 'class ' */
        const char *kw1 = "fun";
        const char *kw2 = "class";
        if (j + 3 <= len && strncmp(text + j, kw1, 3) == 0 && (j + 3 == len || isspace((unsigned char)text[j + 3]))) {
          size_t p = j + 3;
          while (p < len && (text[p] == ' ' || text[p] == '\t'))
            p++;
          /* read identifier */
          size_t start = p;
          if (p < len && (isalpha((unsigned char)text[p]) || text[p] == '_')) {
            p++;
            while (p < len && (isalnum((unsigned char)text[p]) || text[p] == '_'))
              p++;
            size_t n = p - start;
            if (n > 0) {
              char tmp[256];
              size_t copy = (n < sizeof(tmp) - 1) ? n : (sizeof(tmp) - 1);
              memcpy(tmp, text + start, copy);
              tmp[copy] = '\0';
              nl_add(out, tmp);
            }
          }
        } else if (j + 5 <= len && strncmp(text + j, kw2, 5) == 0 && (j + 5 == len || isspace((unsigned char)text[j + 5]))) {
          size_t p = j + 5;
          while (p < len && (text[p] == ' ' || text[p] == '\t'))
            p++;
          /* read identifier */
          size_t start = p;
          if (p < len && (isalpha((unsigned char)text[p]) || text[p] == '_')) {
            p++;
            while (p < len && (isalnum((unsigned char)text[p]) || text[p] == '_'))
              p++;
            size_t n = p - start;
            if (n > 0) {
              char tmp[256];
              size_t copy = (n < sizeof(tmp) - 1) ? n : (sizeof(tmp) - 1);
              memcpy(tmp, text + start, copy);
              tmp[copy] = '\0';
              nl_add(out, tmp);
            }
          }
        }
      }
    }

    /* move forward one char */
    bol = (c == '\n');
    i++;
  }
}

/**
 * @brief Expand include directives in Fun source.
 *
 * Recognizes both `#include "..."` and `include "..."`/`include <...>` at
 * the beginning of a line (after spaces/tabs). Angle-bracket includes search
 * in FUN_LIB_DIR, then DEFAULT_LIB_DIR, then local lib/. Emits span markers of
 * the form `// __include_begin__: <path> [as alias] @line N` to enable later
 * mapping back to original files.
 *
 * @param src           Source code to preprocess.
 * @param current_path  Optional path of the current file for initial marker.
 * @param depth         Recursion depth guard.
 * @return Newly allocated expanded text or NULL on OOM.
 */
static char *preprocess_includes_internal(const char *src, const char *current_path, int depth) {
  if (!src) return NULL;
  if (depth > 64) {
    fprintf(stderr, "Include error: include nesting too deep\n");
    return strdup("");
  }

/* Build-time default, can be overridden by compiler define -DDEFAULT_LIB_DIR=".../" */
#ifndef DEFAULT_LIB_DIR
#define DEFAULT_LIB_DIR "/usr/share/fun/lib/"
#endif

  const char *env_lib = getenv("FUN_LIB_DIR");
  size_t len = strlen(src);
  StrBuf out;
  sb_init(&out);

  /* Preserve shebang on the very first line before inserting the initial marker. */
  size_t shebang_end = 0;
  int shebang_lines = 0;
  if (src[0] == '#' && src[1] == '!') {
    /* find end of line (handle CR, LF, or CRLF) */
    size_t j = 0;
    while (src[j] && src[j] != '\n' && src[j] != '\r') j++;
    /* include line ending */
    if (src[j] == '\r') {
      j++;
      if (src[j] == '\n') j++;
    } else if (src[j] == '\n') {
      j++;
    }
    /* count lines in shebang we are copying */
    for (size_t t = 0; t < j; ++t) if (src[t] == '\n') shebang_lines++;
    if (shebang_lines == 0) shebang_lines = 1; /* single shebang line without LF */
    shebang_end = j;
    sb_append_n(&out, src, shebang_end);
  }

  /* When we know the current file path, emit a leading marker so mapping
   * can always recover the correct file for regions before any include. */
  if (current_path && current_path[0]) {
    sb_append(&out, "// __include_begin__: ");
    sb_append(&out, current_path);
    /* annotate physical base line in the original file */
    char lb[32];
    int base_line = 1 + (shebang_end ? shebang_lines : 0);
    snprintf(lb, sizeof(lb), " @line %d", base_line);
    sb_append(&out, lb);
    sb_append(&out, "\n");
  }
  int in_line = 0, in_block = 0, in_sq = 0, in_dq = 0, esc = 0;
  int bol = 1; /* beginning of line */

  for (size_t i = shebang_end; i < len;) {
    char c = src[i];

    /* Detect include directive at BOL, outside comments/strings */
    if (bol && !in_block && !in_sq && !in_dq) {
      size_t j = i;
      /* skip leading spaces/tabs */
      while (j < len && (src[j] == ' ' || src[j] == '\t'))
        j++;
      size_t k = j;
      if (k < len && src[k] == '#') k++;
      const char *kw = "include";
      size_t kwlen = 7;
      if (k + kwlen <= len && strncmp(src + k, kw, kwlen) == 0) {
        k += kwlen;
        /* next must be space/tab or delimiter */
        while (k < len && (src[k] == ' ' || src[k] == '\t'))
          k++;
        if (k < len && (src[k] == '"' || src[k] == '<')) {
          char opener = src[k];
          char closer = (opener == '"') ? '"' : '>';
          k++;
          size_t path_start = k;
          while (k < len && src[k] != closer)
            k++;
          if (k < len && src[k] == closer) {
            size_t path_len = k - path_start;
            char *path = (char *)malloc(path_len + 1);
            if (path) {
              memcpy(path, src + path_start, path_len);
              path[path_len] = '\0';

              /* parse optional 'as <alias>' then advance to end of line */
              k++;
              char ns[64];
              ns[0] = '\0';
              /* skip spaces/tabs */
              size_t ap = k;
              while (ap < len && (src[ap] == ' ' || src[ap] == '\t'))
                ap++;
              /* optional 'as' */
              const char *askw = "as";
              if (ap + 2 <= len && strncmp(src + ap, askw, 2) == 0 && (ap + 2 == len || isspace((unsigned char)src[ap + 2]))) {
                ap += 2;
                while (ap < len && (src[ap] == ' ' || src[ap] == '\t'))
                  ap++;
                /* read identifier [A-Za-z_][A-Za-z0-9_]* */
                size_t start = ap;
                if (ap < len && (isalpha((unsigned char)src[ap]) || src[ap] == '_')) {
                  ap++;
                  while (ap < len && (isalnum((unsigned char)src[ap]) || src[ap] == '_'))
                    ap++;
                  size_t n = ap - start;
                  size_t copy = (n < sizeof(ns) - 1) ? n : (sizeof(ns) - 1);
                  memcpy(ns, src + start, copy);
                  ns[copy] = '\0';
                }
                /* ignore anything else on line */
              }
              /* advance to end of line */
              k = ap;
              while (k < len && src[k] != '\n')
                k++;
              if (k < len && src[k] == '\n') k++;

              /* resolve file path; for <...> try FUN_LIB_DIR first, then default */
              char resolved[1024];
              char resolved2[1024];
              size_t inc_len = 0;
              char *inc = NULL;

              if (opener == '<') {
                /* Angle-bracket include resolution order:
                 *  1) FUN_LIB_DIR (env), respecting '/' or '\' endings
                 *  2) DEFAULT_LIB_DIR (compile-time define)
                 *  3) "lib/" under current working directory (developer fallback)
                 *
                 * Always assign 'resolved' to the last attempted candidate so errors are informative.
                 */
                resolved[0] = '\0';

                /* 1) FUN_LIB_DIR */
                if (env_lib && env_lib[0]) {
                  size_t elen = strlen(env_lib);
                  char last = env_lib[elen ? (elen - 1) : 0];
                  int needs_sep = !(last == '/' || last == '\\');
                  char sep = (last == '\\') ? '\\' : '/';
                  if (needs_sep)
                    snprintf(resolved, sizeof(resolved), "%s%c%s", env_lib, sep, path);
                  else
                    snprintf(resolved, sizeof(resolved), "%s%s", env_lib, path);
                  inc = read_file_all(resolved, &inc_len);
                }

                /* 2) DEFAULT_LIB_DIR */
                if (!inc) {
                  snprintf(resolved, sizeof(resolved), "%s%s", DEFAULT_LIB_DIR, path);
                  inc = read_file_all(resolved, &inc_len);
                }

                /* 3) project-local dev fallback: lib/<path> */
                if (!inc) {
                  snprintf(resolved, sizeof(resolved), "lib/%s", path);
                  inc = read_file_all(resolved, &inc_len);
                }
              } else {
                /* quoted include: relative path (cwd) */
                snprintf(resolved, sizeof(resolved), "%s", path);
                inc = read_file_all(resolved, &inc_len);
              }
              free(path);

              if (!inc) {
                fprintf(stderr, "Include error: cannot read '%s'\n", resolved[0] ? resolved : "(unresolved)");
                sb_append(&out, "// include error: cannot read ");
                sb_append(&out, resolved[0] ? resolved : "(unresolved)");
                sb_append(&out, "\n");
              } else {
                /* Strip optional UTF-8 BOM and top-of-file shebang from included text before preprocessing */
                const char *startp = inc;
                size_t off = 0;
                if ((unsigned char)inc[0] == 0xEF && (unsigned char)inc[1] == 0xBB && (unsigned char)inc[2] == 0xBF) {
                  off = 3;
                }
                startp = inc + off;
                if (startp[0] == '#' && startp[1] == '!') {
                  /* skip until end of line, handling CR, LF, CRLF */
                  const char *q = startp;
                  while (*q && *q != '\n' && *q != '\r')
                    q++;
                  if (*q == '\r') {
                    q++;
                    if (*q == '\n') q++;
                  } else if (*q == '\n') {
                    q++;
                  }
                  startp = q;
                }
                char *inc_clean = strdup(startp);
                char *exp = preprocess_includes_internal(inc_clean, resolved, depth + 1);
                free(inc);
                free(inc_clean);
                if (exp) {
                  /* mark file origin for better error messages */
                  sb_append(&out, "// __include_begin__: ");
                  sb_append(&out, resolved);
                  if (ns[0] != '\0') {
                    sb_append(&out, " as ");
                    sb_append(&out, ns);
                  }
                  sb_append(&out, " @line 1");
                  sb_append(&out, "\n");

                  /* append expanded included content */
                  sb_append(&out, exp);
                  /* ensure included chunk ends with newline to preserve line structure */
                  if (out.len == 0 || out.buf[out.len - 1] != '\n') sb_append_ch(&out, '\n');

                  /* After including, if we know the parent file path, emit a marker to
                   * resume mapping to the including (parent) file for the subsequent text. */
                  if (current_path && current_path[0]) {
                    sb_append(&out, "// __include_begin__: ");
                    sb_append(&out, current_path);
                    /* compute resume line number in parent: next line after include directive */
                    char lb2[32];
                    /* crude estimate: resume at next physical line */
                    snprintf(lb2, sizeof(lb2), " @line %d", 0); /* will be patched below */
                    /* We need actual parent line number. Compute by scanning from start to 'i' */
                    int parent_line = 1 + (shebang_end ? shebang_lines : 0);
                    for (size_t tt = shebang_end; tt < k; ++tt) if (src[tt] == '\n') parent_line++;
                    snprintf(lb2, sizeof(lb2), " @line %d", parent_line);
                    sb_append(&out, lb2);
                    sb_append(&out, "\n");
                  }

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
      if (c == '\n') {
        in_line = 0;
        bol = 1;
      } else {
        bol = 0;
      }
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
      if (!esc && c == '\\') {
        esc = 1;
        i++;
        bol = 0;
        continue;
      }
      if (!esc && c == '\'') {
        in_sq = 0;
      }
      esc = 0;
      bol = (c == '\n') ? 1 : 0;
      i++;
      continue;
    }
    if (in_dq) {
      sb_append_ch(&out, c);
      if (!esc && c == '\\') {
        esc = 1;
        i++;
        bol = 0;
        continue;
      }
      if (!esc && c == '"') {
        in_dq = 0;
      }
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

/**
 * @brief Public wrapper to preprocess includes without a current path.
 */
char *preprocess_includes(const char *src) {
  return preprocess_includes_internal(src, NULL, 0);
}

/* Variant with known current file path to allow precise resume markers. */
/**
 * @brief Preprocess includes with a known file path to improve span markers.
 */
char *preprocess_includes_with_path(const char *src, const char *current_path) {
  return preprocess_includes_internal(src, current_path, 0);
}

/*
 * Public helper: map a line number in the include-expanded top-level source file
 * back to the original included file path and inner line number, using the
 * `// __include_begin__: <path>[ as <alias>]` markers injected by the
 * preprocessor. Returns 1 on success and fills out_path/out_line; 0 otherwise.
 */
/**
 * @brief Map a line number in expanded source back to original include path/line.
 *
 * Scans the expanded text for the nearest preceding `__include_begin__` marker
 * and computes the corresponding inner line number.
 *
 * @param path           Path to the original top-level file that was expanded.
 * @param line           1-based line number in the expanded text.
 * @param out_path       Output buffer for the resolved file path.
 * @param out_path_cap   Capacity of out_path.
 * @param out_line       Receives 1-based line number within resolved file.
 * @return 1 on success, 0 on failure.
 */
int map_expanded_line_to_include_path(const char *path, int line,
                                      char *out_path, size_t out_path_cap,
                                      int *out_line) {
  if (!path || line <= 0 || !out_path || out_path_cap == 0 || !out_line) return 0;
  out_path[0] = '\0';
  *out_line = line;

  /* read original top-level file */
  size_t fsz = 0;
  char *orig = read_file_all(path, &fsz);
  if (!orig) return 0;

  char *prep = preprocess_includes_internal(orig, path, 0);
  free(orig);
  if (!prep) return 0;

  /* find start offset of requested 1-based line */
  size_t len = strlen(prep);
  size_t pos = 0;
  int cur = 1;
  while (pos < len && cur < line) {
    if (prep[pos] == '\n') cur++;
    pos++;
  }
  if (cur != line) { free(prep); return 0; }

  /* scan backward to find the nearest include/resume marker line */
  const char *marker = "// __include_begin__: ";
  size_t mlen = strlen(marker);
  size_t scan = pos;
  while (scan > 0) {
    /* find start of current line */
    size_t ls = scan;
    while (ls > 0 && prep[ls - 1] != '\n') ls--;
    /* check if this line starts with marker */
    if (ls + mlen <= len && strncmp(prep + ls, marker, mlen) == 0) {
      /* Parse marker line: path [as alias] [@line N] */
      size_t p = ls + mlen;
      size_t eol = p;
      while (eol < len && prep[eol] != '\n') eol++;
      /* find separators */
      size_t pos_as = eol, pos_line = eol;
      for (size_t t = p; t + 3 < eol; ++t) {
        if (prep[t] == ' ' && strncmp(prep + t, " as ", 4) == 0) { pos_as = t; break; }
      }
      for (size_t t = p; t + 6 < eol; ++t) {
        if (prep[t] == ' ' && strncmp(prep + t, " @line ", 7) == 0) { pos_line = t; break; }
      }
      size_t path_end = pos_as < pos_line ? pos_as : pos_line;
      if (path_end < p) path_end = eol;
      size_t copy = (path_end - p) < (out_path_cap - 1) ? (path_end - p) : (out_path_cap - 1);
      memcpy(out_path, prep + p, copy);
      out_path[copy] = '\0';

      /* parse optional base line number */
      int base_line = 1;
      if (pos_line < eol) {
        size_t num_start = pos_line + 7; /* after ' @line ' */
        int v = 0;
        while (num_start < eol && prep[num_start] == ' ') num_start++;
        while (num_start < eol && prep[num_start] >= '0' && prep[num_start] <= '9') {
          v = v * 10 + (prep[num_start] - '0');
          num_start++;
        }
        if (v > 0) base_line = v;
      }

      /* determine the span: from the end of this marker line to the start of the next marker line */
      size_t q = eol;
      if (q < len && prep[q] == '\n') q++;
      size_t span_start = q;
      size_t span_end = len;
      size_t fwd = q;
      while (fwd < len) {
        /* find start of next line */
        size_t ls2 = fwd;
        while (ls2 > 0 && prep[ls2 - 1] != '\n') ls2--;
        if (ls2 + mlen <= len && strncmp(prep + ls2, marker, mlen) == 0) {
          span_end = ls2; /* region ends right before the next marker */
          break;
        }
        while (fwd < len && prep[fwd] != '\n') fwd++;
        if (fwd < len && prep[fwd] == '\n') fwd++;
      }

      /* If the requested position is not within [span_start, span_end), this
       * begin marker does not apply; continue scanning backward. */
      if (!(pos >= span_start && pos < span_end)) {
        /* not inside this include span */
        goto next_scan_back;
      }

      /* compute inner line as number of newlines from span_start to current pos */
      int inner = 1;
      size_t cnt = span_start;
      while (cnt < pos) { if (prep[cnt] == '\n') inner++; cnt++; }
      *out_line = base_line + inner - 1;
      free(prep);
      return 1;
    }
    if (ls == 0) break;
next_scan_back:
    scan = (ls > 0) ? (ls - 1) : 0;
  }

  free(prep);
  return 0;
}

/* Float literal parser: supports decimal and scientific notation. Returns parsed double and advances pos on success. */
/**
 * @brief Parse a floating-point literal (supports . and scientific notation).
 * @param ok Set to 1 on success, 0 otherwise.
 * @return Parsed double value.
 */
static double parse_float_literal_value(const char *src, size_t len, size_t *pos, int *ok) {
  size_t p = *pos;
  skip_spaces(src, len, &p);
  size_t start = p;
  int saw_digit = 0;
  int saw_dot = 0;
  int saw_exp = 0;

  /* optional sign */
  if (p < len && (src[p] == '+' || src[p] == '-')) p++;

  /* integer part */
  while (p < len && isdigit((unsigned char)src[p])) {
    p++;
    saw_digit = 1;
  }

  /* fractional part */
  if (p < len && src[p] == '.') {
    saw_dot = 1;
    p++;
    while (p < len && isdigit((unsigned char)src[p])) {
      p++;
      saw_digit = 1;
    }
  }

  /* exponent part */
  if (p < len && (src[p] == 'e' || src[p] == 'E')) {
    saw_exp = 1;
    size_t epos = p + 1;
    if (epos < len && (src[epos] == '+' || src[epos] == '-')) epos++;
    size_t digits_start = epos;
    while (epos < len && isdigit((unsigned char)src[epos])) {
      epos++;
    }
    if (epos == digits_start) {
      /* no digits after exponent -> not a float */
      *ok = 0;
      return 0.0;
    }
    p = epos;
  }

  if (!saw_digit || (!saw_dot && !saw_exp)) {
    *ok = 0;
    return 0.0;
  }

  /* Create temporary buffer to parse with strtod safely */
  size_t n = p - start;
  char *tmp = (char *)malloc(n + 1);
  if (!tmp) {
    *ok = 0;
    return 0.0;
  }
  memcpy(tmp, src + start, n);
  tmp[n] = '\0';

  char *endp = NULL;
  double dv = strtod(tmp, &endp);
  if (!endp || *endp != '\0') {
    free(tmp);
    *ok = 0;
    return 0.0;
  }

  *pos = p;
  *ok = 1;
  free(tmp);
  return dv;
}
