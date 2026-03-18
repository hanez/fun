/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytecode.h"
#include "parser.h"

static void usage(const char *prog) {
  fprintf(stderr, "Usage: %s [--fix] <file1.fun> [file2.fun ...]\n", prog);
}

/* Read entire file into a malloc'd buffer terminated with '\0'. Returns 1 on success. */
static int read_all(const char *path, char **out_buf, size_t *out_len) {
  *out_buf = NULL;
  *out_len = 0;
  FILE *f = fopen(path, "rb");
  if (!f) return 0;
  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return 0;
  }
  long sz = ftell(f);
  if (sz < 0) {
    fclose(f);
    return 0;
  }
  if (fseek(f, 0, SEEK_SET) != 0) {
    fclose(f);
    return 0;
  }
  char *buf = (char *)malloc((size_t)sz + 1);
  if (!buf) {
    fclose(f);
    return 0;
  }
  size_t rd = fread(buf, 1, (size_t)sz, f);
  fclose(f);
  buf[rd] = '\0';
  *out_buf = buf;
  *out_len = rd;
  return 1;
}

/* Write buffer to file atomically-ish (overwrite). */
static int write_all(const char *path, const char *buf, size_t len) {
  FILE *f = fopen(path, "wb");
  if (!f) return 0;
  size_t wr = fwrite(buf, 1, len, f);
  int ok = (wr == len);
  fclose(f);
  return ok;
}

/* Check if c is a word constituent (identifier char) */
static int is_word(int c) {
  return (c == '_' || isalnum(c));
}

/* Apply auto-fixes to the given source text. Returns newly malloc'd buffer and new length.
 * Idempotent, focuses on parser-related constraints: 2-space indents, no tab indents,
 * CRLF->LF, trim trailing spaces, ensure final newline, normalize 'sint*' to 'int*'.
 */
static char *apply_fixes(const char *src, size_t len, size_t *out_len) {
  /* First pass: normalize line endings to LF and compute an upper bound size */
  /* We'll build into a dynamic buffer that grows if needed. */
  size_t cap = len + 32;
  char *out = (char *)malloc(cap);
  if (!out) return NULL;
  size_t o = 0;

  size_t i = 0;
  while (i < len) {
    /* Grow if needed */
    if (o + 16 >= cap) {
      cap = cap * 2 + 64;
      char *n = (char *)realloc(out, cap);
      if (!n) {
        free(out);
        return NULL;
      }
      out = n;
    }

    /* Start of line: handle indentation fixes */
    size_t line_start_out = o;
    int at_bol = 1;
    int spaces = 0;

    /* Process indentation: convert tabs to two spaces and count spaces */
    while (i < len) {
      char c = src[i];
      if (c == '\r' || c == '\n') break; /* empty line */
      if (at_bol && (c == ' ' || c == '\t')) {
        if (c == ' ') {
          spaces++;
        } else if (c == '\t') {
          spaces += 2;
        } /* replace leading tab with 2 spaces */
        i++;
        continue;
      }
      break;
    }
    /* Emit normalized indentation: nearest even multiple (round up if odd) */
    if (at_bol) {
      int normalized = (spaces % 2 == 0) ? spaces : spaces + 1;
      for (int k = 0; k < normalized; k++) {
        if (o + 1 >= cap) {
          cap = cap * 2 + 64;
          char *n = realloc(out, cap);
          if (!n) {
            free(out);
            return NULL;
          }
          out = n;
        }
        out[o++] = ' ';
      }
      at_bol = 0;
    }

    /* Copy rest of line, performing CRLF->LF, trimming trailing spaces later, and identifier normalization */
    size_t line_content_start = i;
    size_t line_end_i = i;
    int saw_cr = 0;
    while (line_end_i < len) {
      char c = src[line_end_i];
      if (c == '\r') {
        saw_cr = 1;
        break;
      }
      if (c == '\n') break;
      line_end_i++;
    }
    /* Now we have [i, line_end_i) content. We'll trim trailing spaces. */
    size_t trim_end = line_end_i;
    while (trim_end > i && src[trim_end - 1] == ' ')
      trim_end--;

    /* Identifier normalization within the line content: replace sint* -> int* when at word boundaries */
    size_t p = i;
    while (p < trim_end) {
      /* Patterns to check */
      const char *keys[] = {"sint8", "sint16", "sint32", "sint64"};
      const char *vals[] = {"int8", "int16", "int32", "int64"};
      int replaced = 0;
      for (int idx = 0; idx < 4; idx++) {
        const char *k = keys[idx];
        size_t klen = strlen(k);
        if (p + klen <= trim_end && strncmp(&src[p], k, klen) == 0) {
          int left_ok = (p == i) || !is_word((unsigned char)src[p - 1]);
          int right_ok = (p + klen == trim_end) || !is_word((unsigned char)src[p + klen]);
          if (left_ok && right_ok) {
            const char *v = vals[idx];
            size_t vlen = strlen(v);
            if (o + vlen + 16 >= cap) {
              cap = cap * 2 + 64;
              char *n = realloc(out, cap);
              if (!n) {
                free(out);
                return NULL;
              }
              out = n;
            }
            memcpy(&out[o], v, vlen);
            o += vlen;
            p += klen;
            replaced = 1;
            break;
          }
        }
      }
      if (replaced) continue;
      if (o + 1 + 16 >= cap) {
        cap = cap * 2 + 64;
        char *n = realloc(out, cap);
        if (!n) {
          free(out);
          return NULL;
        }
        out = n;
      }
      out[o++] = src[p++];
    }

    /* EOL: write single LF if we saw any EOL marker, else we'll add at the very end */
    if (line_end_i < len) {
      /* consume CRLF or CR */
      if (src[line_end_i] == '\r') {
        line_end_i++;
        if (line_end_i < len && src[line_end_i] == '\n') line_end_i++;
      } else if (src[line_end_i] == '\n') {
        line_end_i++;
      }
      if (o + 1 >= cap) {
        cap = cap * 2 + 64;
        char *n = realloc(out, cap);
        if (!n) {
          free(out);
          return NULL;
        }
        out = n;
      }
      out[o++] = '\n';
    }

    i = line_end_i;
    (void)line_start_out;
    (void)line_content_start;
    (void)saw_cr;
  }

  /* Ensure file ends with a single LF */
  if (o == 0 || out[o - 1] != '\n') {
    if (o + 1 >= cap) {
      cap = cap + 8;
      char *n = (char *)realloc(out, cap);
      if (!n) {
        free(out);
        return NULL;
      }
      out = n;
    }
    out[o++] = '\n';
  }

  *out_len = o;
  /* shrink to fit */
  char *shr = (char *)realloc(out, o + 1);
  if (shr) out = shr;
  out[o] = '\0';
  return out;
}

int main(int argc, char **argv) {
  int do_fix = 0;
  int first_file_arg = 1;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--fix") == 0) {
      do_fix = 1;
      first_file_arg++;
    } else
      break;
  }

  if (argc < 1 + (do_fix ? 1 : 0) + 1) {
    usage(argv[0]);
    return 2;
  }

  int hadError = 0;

  for (int i = first_file_arg; i < argc; i++) {
    const char *path = argv[i];

    if (do_fix) {
      /* Read, fix, validate, and write back if parse succeeds */
      char *orig = NULL;
      size_t orig_len = 0;
      if (!read_all(path, &orig, &orig_len)) {
        fprintf(stderr, "%s: cannot read file\n", path);
        hadError = 1;
        continue;
      }
      size_t fixed_len = 0;
      char *fixed = apply_fixes(orig, orig_len, &fixed_len);
      if (!fixed) {
        fprintf(stderr, "%s: out of memory while fixing\n", path);
        free(orig);
        hadError = 1;
        continue;
      }

      /* Validate by parsing the fixed source using parse_string_to_bytecode */
      Bytecode *test_bc = parse_string_to_bytecode(fixed);
      if (!test_bc) {
        char msg[512];
        int line = 0, col = 0;
        if (parser_last_error(msg, (unsigned long)sizeof msg, &line, &col)) {
          fprintf(stderr, "%s:%d:%d: syntax error after --fix attempt: %s\n", path, line, col, msg);
        } else {
          fprintf(stderr, "%s: syntax error (unknown) after --fix attempt\n", path);
        }
        hadError = 1;
        free(orig);
        free(fixed);
        continue;
      }
      bytecode_free(test_bc);

      /* If content differs or original did not end LF, write back */
      int changed = 0;
      if (orig_len != fixed_len || memcmp(orig, fixed, (orig_len < fixed_len ? orig_len : fixed_len)) != 0) changed = 1;
      if (changed) {
        if (!write_all(path, fixed, fixed_len)) {
          fprintf(stderr, "%s: failed to write fixed file\n", path);
          hadError = 1;
          free(orig);
          free(fixed);
          continue;
        }
      }
      free(orig);
      free(fixed);
    }

    /* Finally, run the normal syntax check on the (possibly fixed) file */
    Bytecode *bc = parse_file_to_bytecode(path);
    if (!bc) {
      char msg[512];
      int line = 0, col = 0;
      if (parser_last_error(msg, (unsigned long)sizeof msg, &line, &col)) {
        fprintf(stderr, "%s:%d:%d: syntax error: %s\n", path, line, col, msg);
      } else {
        fprintf(stderr, "%s: syntax error (unknown)\n", path);
      }
      hadError = 1;
      continue; // keep checking remaining files
    }

    fprintf(stdout, "%s: OK\n", path);
    bytecode_free(bc);
  }

  return hadError ? 1 : 0;
}
