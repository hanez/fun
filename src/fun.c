/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

/**
* @file fun.c
 * @brief Main entry point for the Fun language interpreter.
 *
 * This file contains the core logic for the Fun interpreter, including:
 * - Command-line argument parsing.
 * - File and REPL mode handling.
 * - Utility functions for parsing and executing Fun code.
 *
 * Key Functions:
 * - `is_blank_line`: Checks if a line is blank or contains only whitespace.
 * - `lstrip`: Strips leading whitespace from a string.
 * - `ends_with_opener`: Determines if a line ends with a continuation operator.
 * - `compute_open_indent_blocks`: Computes the number of open indentation blocks.
 * - `buffer_looks_incomplete`: Detects if a buffer contains incomplete code.
 *
 * Error Handling:
 * - Exits with an error if file loading or parsing fails.
 *
 * Example:
 * $ fun script.fun
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */


#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* REPL line editing and completion (POSIX) */
#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#endif

/* --------- REPL history (in-memory + file integration) ---------- */
enum { RL_HIST_MAX = 1000 };
static char *rl_hist[RL_HIST_MAX];
static int rl_count = 0;

/* last history entry (or NULL) */
static const char* rl_hist_last(void) {
    if (rl_count <= 0) return NULL;
    return rl_hist[rl_count - 1];
}

/* add one line to in-memory history (without trailing newline), dedup consecutive */
static void rl_hist_add(const char *s) {
    if (!s) return;
    size_t n = strlen(s);
    /* strip single trailing newline if present */
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) n--;
    if (n == 0) return;
    /* dedupe consecutive identical */
    const char *last = rl_hist_last();
    if (last && strncmp(last, s, n) == 0 && last[n] == '\0') return;

    /* allocate and copy */
    char *cpy = (char*)malloc(n + 1);
    if (!cpy) return;
    memcpy(cpy, s, n);
    cpy[n] = '\0';

    if (rl_count == RL_HIST_MAX) {
        free(rl_hist[0]);
        memmove(&rl_hist[0], &rl_hist[1], sizeof(rl_hist[0]) * (RL_HIST_MAX - 1));
        rl_count--;
    }
    rl_hist[rl_count++] = cpy;
}

/* preload history from file (one line per entry) */
static void rl_hist_load_file(const char *path) {
    if (!path) return;
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        rl_hist_add(line);
    }
    fclose(f);
}

#ifndef _WIN32
static struct termios g_orig_tios;
static int g_raw_enabled = 0;

static void repl_disable_raw(void) {
    if (g_raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig_tios);
        g_raw_enabled = 0;
    }
}
static int repl_enable_raw(void) {
    if (!isatty(STDIN_FILENO)) return 0;
    if (g_raw_enabled) return 1;
    if (tcgetattr(STDIN_FILENO, &g_orig_tios) == -1) return 0;
    struct termios raw = g_orig_tios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return 0;
    g_raw_enabled = 1;
    return 1;
}

/* return 1 if path is a directory, else 0 */
static int is_dir_path(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

/* Compute longest common prefix of a set of strings (starting from offset base_len) */
static size_t lcp_suffix(const char **names, int count, size_t base_len) {
    if (count <= 0) return base_len;
    size_t lcp = (size_t)-1;
    for (int i = 0; i < count; ++i) {
        size_t nlen = strlen(names[i]);
        size_t cur = 0;
        /* compare suffix part starting at base_len */
        while (base_len + cur < nlen) {
            char c = names[i][base_len + cur];
            int ok = 1;
            for (int j = 0; j < count; ++j) {
                size_t jlen = strlen(names[j]);
                if (base_len + cur >= jlen || names[j][base_len + cur] != c) { ok = 0; break; }
            }
            if (!ok) break;
            cur++;
        }
        if (lcp == (size_t)-1 || cur < lcp) lcp = cur;
    }
    return base_len + (lcp == (size_t)-1 ? 0 : lcp);
}

/* Expand file path for ":load " completion in-place; returns 1 if buffer changed (redraw) */
static int complete_load_path(char *buf, size_t *len_io) {
    size_t len = *len_io;
    /* Only handle lines starting with ":load" + space(s) */
    if (len < 5) return 0;
    /* find first non-space after ":load" */
    const char *p = buf;
    while (*p == ' ') p++;
    if (*p != ':') return 0;
    p++;
    if (strncmp(p, "load", 4) != 0) return 0;
    p += 4;
    while (*p == ' ' || *p == '\t') p++;
    size_t arg_off = (size_t)(p - buf);
    if (arg_off > len) return 0;

    /* current prefix from arg_off .. len */
    char prefix[PATH_MAX];
    size_t plen = 0;
    if (len - arg_off >= sizeof(prefix)) plen = sizeof(prefix) - 1;
    else plen = len - arg_off;
    memcpy(prefix, buf + arg_off, plen);
    prefix[plen] = '\0';

    /* handle ~ */
    char expanded[PATH_MAX];
    if (prefix[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) home = getenv("USERPROFILE");
        if (home) {
            snprintf(expanded, sizeof(expanded), "%s%s", home, prefix + 1);
        } else {
            snprintf(expanded, sizeof(expanded), "%s", prefix);
        }
    } else {
        snprintf(expanded, sizeof(expanded), "%s", prefix);
    }

    /* split dir/base */
    char dirpart[PATH_MAX], base[PATH_MAX];
    const char *slash = strrchr(expanded, '/');
    if (slash) {
        size_t dlen = (size_t)(slash - expanded);
        if (dlen == 0) { strcpy(dirpart, "/"); }
        else { memcpy(dirpart, expanded, dlen); dirpart[dlen] = '\0'; }
        snprintf(base, sizeof(base), "%s", slash + 1);
    } else {
        strcpy(dirpart, ".");
        snprintf(base, sizeof(base), "%s", expanded);
    }

    DIR *dp = opendir(dirpart);
    if (!dp) { fputc('\a', stdout); fflush(stdout); return 0; }

    /* collect matches */
    const char *names[1024];
    char storage[1024][NAME_MAX + 1];
    int count = 0;
    struct dirent *de;
    size_t blen = strlen(base);
    while ((de = readdir(dp)) != NULL) {
        if (blen == 0 || strncmp(de->d_name, base, blen) == 0) {
            if (count < (int)(sizeof(names)/sizeof(names[0]))) {
                snprintf(storage[count], sizeof(storage[count]), "%s", de->d_name);
                names[count] = storage[count];
                count++;
            }
        }
    }
    closedir(dp);

    if (count == 0) { fputc('\a', stdout); fflush(stdout); return 0; }

    /* compute completion */
    size_t new_pref_len = lcp_suffix(names, count, blen);
    int changed = 0;

    /* build completed path: dirpart + "/" + names[0] prefix */
    char completed[PATH_MAX];
    if (slash) {
        char head[PATH_MAX];
        memcpy(head, expanded, (size_t)(slash - expanded + 1));
        head[slash - expanded + 1] = '\0';
        strncpy(completed, head, sizeof(completed));
    } else {
        snprintf(completed, sizeof(completed), "%s", "");
    }

    /* append base + additional common chars */
    strncat(completed, base, sizeof(completed) - strlen(completed) - 1);
    if (new_pref_len > blen) {
        strncat(completed, names[0] + blen, (new_pref_len - blen));
        changed = 1;
    } else if (count == 1) {
        /* unique -> complete fully */
        size_t extra = strlen(names[0]) - blen;
        strncat(completed, names[0] + blen, extra);
        changed = 1;
    }

    /* if unique and it's a directory, append trailing slash */
    if (count == 1) {
        char test[PATH_MAX];
        if (slash) snprintf(test, sizeof(test), "%.*s/%s", (int)(slash - expanded), expanded, names[0]);
        else snprintf(test, sizeof(test), "%s", names[0]);
        if (is_dir_path(test)) {
            strncat(completed, "/", sizeof(completed) - strlen(completed) - 1);
        }
    }

    /* If no progress and multiple matches, list them */
    if (!changed && count > 1) {
        putchar('\n');
        for (int i = 0; i < count; ++i) {
            fputs(names[i], stdout);
            fputc(((i + 1) % 6 == 0) ? '\n' : '\t', stdout);
        }
        if (count % 6 != 0) putchar('\n');
        fflush(stdout);
        return 0;
    }

    /* write back into buffer after arg_off */
    size_t comp_len = strlen(completed);
    if (arg_off + comp_len >= PATH_MAX - 1) comp_len = PATH_MAX - 2 - arg_off;
    memcpy(buf + arg_off, completed, comp_len);
    *len_io = arg_off + comp_len;
    buf[*len_io] = '\0';
    return 1;
}

/* Read one line with prompt, handling backspace, Up/Down history and :load path completion */
static int read_line_edit(char *out, size_t out_cap, const char *prompt) {
#ifdef _WIN32
    /* fallback to standard fgets on Windows */
    if (prompt) fputs(prompt, stdout), fflush(stdout);
    if (!fgets(out, (int)out_cap, stdin)) return 0;
    return 1;
#else
    if (prompt) fputs(prompt, stdout), fflush(stdout);
    if (!repl_enable_raw()) {
        /* no raw -> fallback */
        if (!fgets(out, (int)out_cap, stdin)) return 0;
        return 1;
    }

    size_t len = 0;
    int hist_pos = rl_count;  /* rl_count means "editing current input", 0..rl_count-1 indexes history entries */
    char saved_current[4096];
    int has_saved = 0;

    #define RL_REDRAW() do { \
        fputc('\r', stdout); \
        if (prompt) fputs(prompt, stdout); \
        fwrite(out, 1, len, stdout); \
        fputs("\x1b[K", stdout); \
        fflush(stdout); \
    } while(0)

    for (;;) {
        int ch = getchar();
        if (ch == EOF) { repl_disable_raw(); return 0; }

        if (ch == '\r' || ch == '\n') {
            fputc('\n', stdout);
            if (len + 1 < out_cap) {
                out[len++] = '\n';
                out[len] = '\0';
            } else {
                out[len] = '\0';
            }
            /* add to in-memory history */
            rl_hist_add(out);
            repl_disable_raw();
            return 1;
        } else if (ch == 27) { /* ESC sequence */
            int c1 = getchar();
            if (c1 == '[') {
                int c2 = getchar();
                if (c2 == 'A') { /* Up */
                    if (!has_saved) { /* save current line to restore on Down past newest */
                        size_t sl = len < sizeof(saved_current) - 1 ? len : sizeof(saved_current) - 1;
                        memcpy(saved_current, out, sl);
                        saved_current[sl] = '\0';
                        has_saved = 1;
                    }
                    if (hist_pos > 0) {
                        hist_pos--;
                        const char *h = rl_hist[hist_pos];
                        size_t hl = strlen(h);
                        if (hl >= out_cap) hl = out_cap - 1;
                        memcpy(out, h, hl);
                        out[hl] = '\0';
                        len = hl;
                        RL_REDRAW();
                    } else {
                        fputc('\a', stdout); fflush(stdout);
                    }
                } else if (c2 == 'B') { /* Down */
                    if (hist_pos < rl_count) {
                        hist_pos++;
                        if (hist_pos == rl_count) {
                            /* restore saved current */
                            if (has_saved) {
                                size_t hl = strlen(saved_current);
                                if (hl >= out_cap) hl = out_cap - 1;
                                memcpy(out, saved_current, hl);
                                out[hl] = '\0';
                                len = hl;
                            } else {
                                len = 0; out[0] = '\0';
                            }
                        } else {
                            const char *h = rl_hist[hist_pos];
                            size_t hl = strlen(h);
                            if (hl >= out_cap) hl = out_cap - 1;
                            memcpy(out, h, hl);
                            out[hl] = '\0';
                            len = hl;
                        }
                        RL_REDRAW();
                    } else {
                        fputc('\a', stdout); fflush(stdout);
                    }
                } else {
                    /* ignore other CSI sequences */
                }
            }
        } else if (ch == 127 || ch == 8) {
            /* backspace */
            if (len > 0) {
                len--;
                out[len] = '\0';
                /* efficient backspace */
                fputs("\b \b", stdout);
                fflush(stdout);
            } else {
                fputc('\a', stdout);
                fflush(stdout);
            }
        } else if (ch == '\t') {
            /* only attempt completion for ':load ' */
            out[len] = '\0';
            size_t newlen = len;
            if (complete_load_path(out, &newlen)) {
                len = newlen;
            }
            RL_REDRAW();
        } else if (ch >= 32 && ch <= 126) {
            if (len + 1 < out_cap) {
                out[len++] = (char)ch;
                out[len] = '\0';
                fputc(ch, stdout);
                fflush(stdout);
                /* when typing after navigating history, we are editing current line; reset hist_pos to end */
                hist_pos = rl_count;
            } else {
                fputc('\a', stdout);
                fflush(stdout);
            }
        } else {
            /* ignore other control characters */
        }
    }
#endif
}
#endif /* !_WIN32 */

#ifndef FUN_VERSION
#define FUN_VERSION "0.0.0-dev"
#endif

/* ---------- Small utilities ---------- */

static int is_blank_line(const char *s) {
    for (const char *p = s; *p; ++p) {
        if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') return 0;
    }
    return 1;
}

static const char* lstrip(const char *s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}

static int ends_with_opener(const char *line) {
    /* if a line ends with a likely continuation operator, keep reading */
    size_t n = strlen(line);
    while (n > 0 && (line[n-1] == ' ' || line[n-1] == '\t' || line[n-1] == '\r' || line[n-1] == '\n')) n--;
    if (n == 0) return 0;
    char c = line[n-1];
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
        c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|' || c == ',') {
        return 1;
    }
    return 0;
}

static int starts_with_kw(const char *s, const char *kw) {
    s = lstrip(s);
    size_t k = strlen(kw);
    if (strncmp(s, kw, k) != 0) return 0;
    char next = s[k];
    /* word boundary: end, space, paren */
    if (next == 0 || next == ' ' || next == '\t' || next == '(' || next == '\r' || next == '\n') return 1;
    return 0;
}

/* Compute how many indentation levels (2 spaces per level) are still open.
 * Only counts significant lines (non-blank, not comment-only),
 * and ignores text inside \/* ... *\/ block comments. */
static int compute_open_indent_blocks(const char *buf) {
    int in_block_comment = 0;
    int open = 0;
    int have_baseline = 0;
    int cur = 0; /* current indent level in units of 2 spaces */

    const char *p = buf;
    while (*p) {
        /* start of line */
        const char *line = p;
        /* find end of line */
        while (*p && *p != '\n') p++;
        const char *line_end = p;

        /* advance p past newline if present */
        if (*p == '\n') p++;

        if (in_block_comment) {
            /* look for end of block comment marker on this line */
            const char *q = line;
            while (q < line_end) {
                if (q + 1 < line_end && q[0] == '*' && q[1] == '/') { in_block_comment = 0; q += 2; break; }
                q++;
            }
            /* whole line is inside block comment -> skip */
            if (in_block_comment) continue;
        }

        /* measure leading spaces */
        const char *s = line;
        int spaces = 0;
        while (s < line_end && *s == ' ') { spaces++; s++; }
        /* skip tabs for safety but don't treat as indent units */
        while (s < line_end && *s == '\t') { s++; }

        /* detect line comments and block comment start */
        const char *t = s;
        /* blank line */
        if (t >= line_end) continue;

        if ((t + 1) <= line_end && t[0] == '/' && (t + 1 < line_end && (t[1] == '/' || t[1] == '*'))) {
            if (t[1] == '/') {
                /* // comment-only line */
                continue;
            } else if (t[1] == '*') {
                /* start of block comment */
                in_block_comment = 1;
                continue;
            }
        }

        /* significant line */
        int lvl = spaces / 2; /* language uses 2 spaces per level */
        if (!have_baseline) {
            cur = lvl;
            have_baseline = 1;
            continue;
        }
        if (lvl > cur) {
            open += (lvl - cur);
        } else if (lvl < cur) {
            int dec = (cur - lvl);
            if (dec > open) open = 0;
            else open -= dec;
        }
        cur = lvl;
    }
    return open;
}

/* Detect if current buffer looks incomplete:
 * - Unclosed quotes (\' or \") with escapes
 * - Unclosed block comment \/* ... *\/
 * - Parentheses not balanced
 * - Last significant line is a block header (if/else/while/for/fun)
 * - Line ends with an operator/comma
 */
static int buffer_looks_incomplete(const char *buf) {
    int in_single = 0, in_double = 0, escape = 0;
    int in_block_comment = 0, in_line_comment = 0;
    int paren = 0;

    const char *p = buf;
    const char *last_sig_line = NULL;

    while (*p) {
        char c = *p;

        if (in_line_comment) {
            if (c == '\n') in_line_comment = 0;
            p++;
            continue;
        }
        if (in_block_comment) {
            if (c == '*' && p[1] == '/') { in_block_comment = 0; p += 2; continue; }
            p++;
            continue;
        }

        if (!in_single && !in_double) {
            if (c == '/' && p[1] == '/') { in_line_comment = 1; p += 2; continue; }
            if (c == '/' && p[1] == '*') { in_block_comment = 1; p += 2; continue; }
        }

        if (in_single) {
            if (!escape && c == '\\') { escape = 1; p++; continue; }
            if (!escape && c == '\'') { in_single = 0; p++; continue; }
            escape = 0; p++; continue;
        } else if (in_double) {
            if (!escape && c == '\\') { escape = 1; p++; continue; }
            if (!escape && c == '"') { in_double = 0; p++; continue; }
            escape = 0; p++; continue;
        } else {
            if (c == '\'') { in_single = 1; p++; continue; }
            if (c == '"') { in_double = 1; p++; continue; }
            if (c == '(') { paren++; p++; continue; }
            if (c == ')') { if (paren > 0) paren--; p++; continue; }
        }

        /* Track start of the current line excluding blanks */
        if (c == '\n') {
            /* move to char after newline to find next significant line */
            const char *q = p + 1;
            while (*q == ' ' || *q == '\t') q++;
            if (*q && *q != '\n') last_sig_line = q;
        }
        p++;
    }

    /* Determine last significant line (if not set during loop, try from end) */
    if (!last_sig_line) {
        const char *q = buf;
        const char *candidate = NULL;
        while (*q) {
            const char *line_start = q;
            while (*q && *q != '\n') q++;
            /* check non-blank */
            const char *t = line_start;
            while (*t == ' ' || *t == '\t') t++;
            if (*t && *t != '\n' && *t != '\r') candidate = t;
            if (*q == '\n') q++;
        }
        last_sig_line = candidate;
    }

    if (in_single || in_double || in_block_comment || paren > 0) return 1;

    if (last_sig_line) {
        if (starts_with_kw(last_sig_line, "if") ||
            starts_with_kw(last_sig_line, "else") ||
            starts_with_kw(last_sig_line, "while") ||
            starts_with_kw(last_sig_line, "for") ||
            starts_with_kw(last_sig_line, "fun")) {
            return 1;
        }
        if (ends_with_opener(last_sig_line)) return 1;
    }

    return 0;
}

static void print_usage(const char *prog) {
    printf("Fun %s\n", FUN_VERSION);
    printf("Usage:\n");
    printf("  %s [script.fun]\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("When no script is provided, a REPL starts. Submit an empty line to execute the buffer.\n");
}

static void show_repl_help(void) {
    printf("Commands:\n");
    printf("  :help                  Show this help\n");
    printf("  :quit | :q | :exit     Exit the REPL\n");
    printf("  :reset                 Reset VM state (clears globals)\n");
    printf("  :dump  | :globals      Dump current globals\n");
    printf("  :globals [pattern]     Dump globals filtering by value substring\n");
    printf("  :vars [pattern]        Alias for :globals\n");
    printf("  :clear                 Clear current input buffer\n");
    printf("  :print                 Show current buffer\n");
    printf("  :run                   Execute current buffer immediately\n");
    printf("  :profile               Execute buffer and show timing + instruction count\n");
    printf("  :save <file>           Save current buffer to file\n");
    printf("  :load <file>           Load file into buffer (does not run)\n");
    printf("  :paste [run]           Enter paste mode; end with a single '.' line (optional 'run')\n");
    printf("  :history [N]           Show last N lines of history (default 50)\n");
    printf("  :time on|off|toggle    Toggle/enable/disable timing\n");
    printf("  :env [NAME[=VALUE]]    Get or set environment variable\n");
}

static char *read_entire_file(const char *path, size_t *out_len) {
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

static int write_entire_file(const char *path, const char *data, size_t len) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    size_t n = fwrite(data, 1, len, f);
    fclose(f);
    return n == len;
}

static void print_last_n_lines(const char *path, int n) {
    if (n <= 0) n = 50;
    size_t flen = 0;
    char *content = read_entire_file(path, &flen);
    if (!content) {
        printf("No history available.\n");
        return;
    }
    int lines = 0;
    for (size_t i = flen; i > 0; --i) {
        if (content[i-1] == '\n') {
            lines++;
            if (lines > n) { content[i] = '\0'; printf("%s", content + i); free(content); return; }
        }
    }
    /* fewer lines than n -> print all */
    printf("%s", content);
    free(content);
}

static void append_history(FILE *hist, const char *buffer) {
    if (!hist || !buffer) return;
    /* Separate entries with a trailing newline if not present */
    fputs(buffer, hist);
    if (buffer[0] && buffer[strlen(buffer)-1] != '\n') fputc('\n', hist);
    fflush(hist);
}

/* ---------- Env helpers ---------- */
static void env_show_usage(void) {
    printf("Usage:\n");
    printf("  :env NAME          Show environment variable NAME\n");
    printf("  :env NAME=VALUE    Set environment variable NAME to VALUE\n");
    printf("  :env               Show this usage\n");
}

static void env_get(const char *name) {
    const char *v = getenv(name);
    if (v) printf("%s=%s\n", name, v);
    else printf("%s is not set\n", name);
}

static void env_set(const char *name, const char *value) {
#ifdef _WIN32
    if (_putenv_s(name, value ? value : "") != 0) {
        printf("Failed to set %s\n", name);
    }
#else
    if (setenv(name, value ? value : "", 1) != 0) {
        printf("Failed to set %s\n", name);
    }
#endif
}

/* ---------- Main ---------- */

int main(int argc, char **argv) {
    VM vm;
    vm_init(&vm);

    int repl_timing = 0;

    // CLI flags
    if (argc > 1) {
        const char *arg = argv[1];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(arg, "--version") == 0 || strcmp(arg, "-V") == 0) {
            printf("Fun %s\n", FUN_VERSION);
            return 0;
        }
    }

    // If a script path is provided, parse and run it
    if (argc > 1) {
        const char *path = argv[1];
        Bytecode *bc = parse_file_to_bytecode(path);
        if (!bc) {
            fprintf(stderr, "Failed to compile script: %s\n", path);
            return 1;
        }
        clock_t t0 = 0, t1 = 0;
        if (repl_timing) t0 = clock();
        vm_run(&vm, bc);
        if (repl_timing) {
            t1 = clock();
            double ms = (double)(t1 - t0) * 1000.0 / (double)CLOCKS_PER_SEC;
            printf("[time] %.2f ms\n", ms);
        }

        // Print captured output for user scripts
        vm_print_output(&vm);
        vm_clear_output(&vm);

        bytecode_free(bc);
        return 0;
    }

    // REPL mode
    printf("Fun %s REPL\n", FUN_VERSION);
    printf("Type :help for commands. Submit an empty line to run.\n");

    char *buffer = NULL;
    size_t bufcap = 0;
    size_t buflen = 0;

    /* History setup */
    char hist_path[1024];
    FILE *hist = NULL;
    const char *home = getenv("HOME");
    if (!home) home = getenv("USERPROFILE"); /* Windows fallback */
    if (home) {
        snprintf(hist_path, sizeof(hist_path), "%s/.fun_history", home);
        /* preload history into memory before opening for append */
        rl_hist_load_file(hist_path);
        hist = fopen(hist_path, "a+");
    } else {
        snprintf(hist_path, sizeof(hist_path), ".fun_history");
        rl_hist_load_file(hist_path);
        hist = fopen(hist_path, "a+");
    }

    for (;;) {
        /* ensure temporary null-termination for prompt analysis */
        if (buflen + 1 > bufcap) {
            size_t newcap = bufcap == 0 ? 1024 : bufcap * 2;
            while (newcap < buflen + 1) newcap *= 2;
            buffer = (char*)realloc(buffer, newcap);
            bufcap = newcap;
        }
        buffer[buflen] = '\0';
        int indent_debt = (buflen > 0) ? compute_open_indent_blocks(buffer) : 0;

        /* Build prompt and read a line with editing (backspace, :load path completion) */
        char prompt[64];
        if (buflen == 0) {
            snprintf(prompt, sizeof(prompt), "fun> ");
        } else if (indent_debt > 0) {
            snprintf(prompt, sizeof(prompt), "... %d> ", indent_debt);
        } else {
            snprintf(prompt, sizeof(prompt), "... ");
        }

        char line[4096];
#ifndef _WIN32
        if (!read_line_edit(line, sizeof(line), prompt)) {
            puts("");
            break; // EOF
        }
#else
        fputs(prompt, stdout);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            puts("");
            break;
        }
#endif
        /* Persist input line to history file (dedupe done in rl_hist_add; file stores as-is) */
        if (hist) {
            /* Do not store pure newline lines */
            const char *lp = line;
            int only_nl = 1;
            while (*lp) { if (*lp != '\n' && *lp != '\r') { only_nl = 0; break; } lp++; }
            if (!only_nl) append_history(hist, line);
        }

        /* Command handling (available anytime) */
        if (line[0] == ':') {
            /* extract command and argument */
            char cmd[64] = {0};
            char arg[2048] = {0};
            sscanf(line, ":%63s %2047[^\n]", cmd, arg);

            if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0 || strcmp(cmd, "exit") == 0) {
                break;
            } else if (strcmp(cmd, "help") == 0) {
                show_repl_help();
                continue;
            } else if (strcmp(cmd, "reset") == 0) {
                vm_reset(&vm);
                printf("VM state reset.\n");
                continue;
            } else if (strcmp(cmd, "dump") == 0) {
                vm_dump_globals(&vm);
                continue;
            } else if (strcmp(cmd, "globals") == 0 || strcmp(cmd, "vars") == 0) {
                const char *pattern = lstrip(arg);
                int filtered = (pattern && *pattern);
                printf("=== globals%s%s ===\n", filtered ? " matching '" : "", filtered ? pattern : "");
                if (filtered) printf("'\n");
                for (int i = 0; i < MAX_GLOBALS; ++i) {
                    if (vm.globals[i].type == VAL_NIL) continue;
                    char *sv = value_to_string_alloc(&vm.globals[i]);
                    if (!filtered || (sv && strstr(sv, pattern))) {
                        printf("[%d] %s\n", i, sv ? sv : "nil");
                    }
                    free(sv);
                }
                printf("===============\n");
                continue;
            } else if (strcmp(cmd, "clear") == 0) {
                buflen = 0;
                printf("(buffer cleared)\n");
                continue;
            } else if (strcmp(cmd, "print") == 0) {
                if (buflen == 0) printf("(buffer empty)\n");
                else {
                    /* ensure temp null-terminated output */
                    if (buflen >= bufcap) {
                        buffer = (char*)realloc(buffer, buflen + 1);
                        bufcap = buflen + 1;
                    }
                    buffer[buflen] = '\0';
                    printf("%s", buffer);
                    if (buflen > 0 && buffer[buflen-1] != '\n') printf("\n");
                }
                continue;
            } else if (strcmp(cmd, "run") == 0 || strcmp(cmd, "profile") == 0) {
                if (buflen == 0) {
                    printf("(buffer empty)\n");
                    continue;
                }
                /* ensure null-terminated */
                if (buflen + 1 > bufcap) {
                    buffer = (char*)realloc(buffer, buflen + 1);
                    bufcap = buflen + 1;
                }
                buffer[buflen] = '\0';

                clock_t t_parse0 = 0, t_parse1 = 0, t_run0 = 0, t_run1 = 0;
                if (strcmp(cmd, "profile") == 0) t_parse0 = clock();
                Bytecode *bc = parse_string_to_bytecode(buffer);
                if (strcmp(cmd, "profile") == 0) t_parse1 = clock();

                if (bc) {
                    if (repl_timing || strcmp(cmd, "profile") == 0) t_run0 = clock();
                    vm_run(&vm, bc);
                    if (repl_timing || strcmp(cmd, "profile") == 0) t_run1 = clock();

                    if (strcmp(cmd, "profile") == 0) {
                        double ms_parse = (double)(t_parse1 - t_parse0) * 1000.0 / (double)CLOCKS_PER_SEC;
                        double ms_run   = (double)(t_run1 - t_run0)   * 1000.0 / (double)CLOCKS_PER_SEC;
                        printf("[profile] parse: %.2f ms, run: %.2f ms, total: %.2f ms, instr: %lld\n",
                               ms_parse, ms_run, ms_parse + ms_run, vm.instr_count);
                    } else if (repl_timing) {
                        double ms = (double)(t_run1 - t_run0) * 1000.0 / (double)CLOCKS_PER_SEC;
                        printf("[time] %.2f ms\n", ms);
                    }

                    vm_print_output(&vm);
                    vm_clear_output(&vm);
                    bytecode_free(bc);
                    append_history(hist, buffer);
                } else {
                    int line_no = 0, col_no = 0;
                    char emsg[256];
                    if (parser_last_error(emsg, sizeof(emsg), &line_no, &col_no)) {
                        printf("Parse error at %d:%d: %s\n", line_no, col_no, emsg);
                        int cur_line = 1;
                        const char *p = buffer;
                        while (*p && cur_line < line_no) {
                            if (*p == '\n') cur_line++;
                            p++;
                        }
                        const char *line_start = p;
                        while (*p && *p != '\n') p++;
                        fwrite(line_start, 1, (size_t)(p - line_start), stdout);
                        printf("\n");
                        for (int i = 1; i < col_no; ++i) putchar(' ');
                        printf("^\n");
#ifdef FUN_DEBUG
                        if (hist) {
                            fprintf(hist, "// ERROR %d:%d: %s\n", line_no, col_no, emsg);
                            fflush(hist);
                        }
#endif
                    } else {
                        printf("Parse error.\n");
#ifdef FUN_DEBUG
                        if (hist) {
                            fprintf(hist, "// ERROR: parse error\n");
                            fflush(hist);
                        }
#endif
                    }
                }
                buflen = 0;
                continue;
            } else if (strcmp(cmd, "save") == 0) {
                if (arg[0] == '\0') { printf("Usage: :save <file>\n"); continue; }
                if (buflen == 0) { printf("(buffer empty)\n"); continue; }
                if (!write_entire_file(arg, buffer, buflen)) {
                    printf("Failed to save to '%s'\n", arg);
                } else {
                    printf("Saved %zu bytes to '%s'\n", buflen, arg);
                }
                continue;
            } else if (strcmp(cmd, "load") == 0) {
                if (arg[0] == '\0') { printf("Usage: :load <file>\n"); continue; }
                size_t flen = 0;
                char *filebuf = read_entire_file(arg, &flen);
                if (!filebuf) {
                    printf("Failed to load '%s'\n", arg);
                    continue;
                }
                if (flen + 1 > bufcap) {
                    size_t newcap = flen + 1;
                    buffer = (char*)realloc(buffer, newcap);
                    bufcap = newcap;
                }
                memcpy(buffer, filebuf, flen);
                buflen = flen;
                buffer[buflen] = '\0';
                free(filebuf);
                printf("Loaded %zu bytes into buffer. Use :run or submit an empty line to execute.\n", buflen);
                continue;
            } else if (strcmp(cmd, "paste") == 0) {
                int run_after = 0;
                const char *opt = lstrip(arg);
                if (opt && (strcmp(opt, "run") == 0 || strcmp(opt, "exec") == 0)) run_after = 1;
                printf("(paste mode: end with single '.' line)%s\n", run_after ? " [will run]" : "");
                for (;;) {
                    fputs("... paste> ", stdout);
                    fflush(stdout);
                    char pline[8192];
                    if (!fgets(pline, sizeof(pline), stdin)) { puts(""); break; }
                    if ((strcmp(pline, ".\n") == 0) || (strcmp(pline, ".\r\n") == 0) || (strcmp(pline, ".") == 0)) {
                        break;
                    }
                    size_t pl = strlen(pline);
                    if (buflen + pl + 1 > bufcap) {
                        size_t newcap = bufcap == 0 ? 1024 : bufcap * 2;
                        while (newcap < buflen + pl + 1) newcap *= 2;
                        buffer = (char*)realloc(buffer, newcap);
                        bufcap = newcap;
                    }
                    memcpy(buffer + buflen, pline, pl);
                    buflen += pl;
                }
                if (run_after) {
                    if (buflen + 1 > bufcap) { buffer = (char*)realloc(buffer, buflen + 1); bufcap = buflen + 1; }
                    buffer[buflen] = '\0';
                    Bytecode *bc = parse_string_to_bytecode(buffer);
                    if (bc) {
                        clock_t t0 = clock();
                        vm_run(&vm, bc);
                        clock_t t1 = clock();
                        double ms = (double)(t1 - t0) * 1000.0 / (double)CLOCKS_PER_SEC;
                        printf("[time] %.2f ms\n", ms);
                        vm_print_output(&vm);
                        vm_clear_output(&vm);
                        bytecode_free(bc);
                        append_history(hist, buffer);
                    } else {
                        int line_no = 0, col_no = 0;
                        char emsg[256];
                        if (parser_last_error(emsg, sizeof(emsg), &line_no, &col_no)) {
                            printf("Parse error at %d:%d: %s\n", line_no, col_no, emsg);
#ifdef FUN_DEBUG
                            if (hist) {
                                fprintf(hist, "// ERROR %d:%d: %s\n", line_no, col_no, emsg);
                                fflush(hist);
                            }
#endif
                        } else {
                            printf("Parse error.\n");
#ifdef FUN_DEBUG
                            if (hist) {
                                fprintf(hist, "// ERROR: parse error\n");
                                fflush(hist);
                            }
#endif
                        }
                    }
                    buflen = 0;
                } else {
                    printf("(pasted %zu bytes into buffer)\n", buflen);
                }
                continue;
            } else if (strcmp(cmd, "history") == 0) {
                int n = 50;
                if (arg[0] != '\0') n = atoi(arg);
                if (n <= 0) n = 50;
                print_last_n_lines(hist_path, n);
                continue;
            } else if (strcmp(cmd, "time") == 0) {
                if (strcmp(lstrip(arg), "on") == 0) repl_timing = 1;
                else if (strcmp(lstrip(arg), "off") == 0) repl_timing = 0;
                else if (strcmp(lstrip(arg), "toggle") == 0) repl_timing = !repl_timing;
                else {
                    printf("Usage: :time on|off|toggle (currently %s)\n", repl_timing ? "on" : "off");
                    continue;
                }
                printf("Timing %s\n", repl_timing ? "enabled" : "disabled");
                continue;
            } else if (strcmp(cmd, "env") == 0) {
                const char *spec = lstrip(arg);
                if (!spec || *spec == '\0') {
                    env_show_usage();
                    continue;
                }
                const char *eq = strchr(spec, '=');
                if (!eq) {
                    env_get(spec);
                } else {
                    char name[256];
                    size_t nlen = (size_t)(eq - spec);
                    if (nlen >= sizeof(name)) nlen = sizeof(name) - 1;
                    memcpy(name, spec, nlen);
                    name[nlen] = '\0';
                    const char *val = eq + 1;
                    env_set(name, val);
                }
                continue;
            } else {
                printf("Unknown command. Use :help\n");
                continue;
            }
        }

        /* Empty line -> maybe compile and run accumulated buffer */
        if (is_blank_line(line)) {
            if (buflen == 0) continue; // ignore extra empty lines

            /* Ensure null-terminated buffer */
            if (buflen + 1 > bufcap) {
                buffer = (char*)realloc(buffer, buflen + 1);
                bufcap = buflen + 1;
            }
            buffer[buflen] = '\0';

            /* If buffer looks incomplete or has open indent blocks, keep reading */
            int indent_debt = compute_open_indent_blocks(buffer);
            if (buffer_looks_incomplete(buffer) || indent_debt > 0) {
                if (indent_debt > 0) {
                    printf("(incomplete, open block indent +%d)\n", indent_debt);
                } else {
                    printf("(incomplete, continue typing)\n");
                }
                continue;
            }

            Bytecode *bc = parse_string_to_bytecode(buffer);
            if (bc) {
                clock_t t0 = 0, t1 = 0;
                if (repl_timing) t0 = clock();
                vm_run(&vm, bc);
                if (repl_timing) {
                    t1 = clock();
                    double ms = (double)(t1 - t0) * 1000.0 / (double)CLOCKS_PER_SEC;
                    printf("[time] %.2f ms\n", ms);
                }
                vm_print_output(&vm);
                vm_clear_output(&vm);
                bytecode_free(bc);
                append_history(hist, buffer);
            } else {
                // Better error message with caret
                int line_no = 0, col_no = 0;
                char emsg[256];
                if (parser_last_error(emsg, sizeof(emsg), &line_no, &col_no)) {
                    printf("Parse error at %d:%d: %s\n", line_no, col_no, emsg);
                    // Print the offending line and caret
                    int cur_line = 1;
                    const char *p = buffer;
                    while (*p && cur_line < line_no) {
                        if (*p == '\n') cur_line++;
                        p++;
                    }
                    const char *line_start = p;
                    while (*p && *p != '\n') p++;
                    fwrite(line_start, 1, (size_t)(p - line_start), stdout);
                    printf("\n");
                    for (int i = 1; i < col_no; ++i) putchar(' ');
                    printf("^\n");
#ifdef FUN_DEBUG
                    if (hist) {
                        fprintf(hist, "// ERROR %d:%d: %s\n", line_no, col_no, emsg);
                        fflush(hist);
                    }
#endif
                } else {
                    printf("Parse error.\n");
#ifdef FUN_DEBUG
                    if (hist) {
                        fprintf(hist, "// ERROR: parse error\n");
                        fflush(hist);
                    }
#endif
                }
            }
            // reset buffer
            buflen = 0;
            continue;
        }

        /* Append line to buffer */
        size_t linelen = strlen(line);
        if (buflen + linelen + 1 > bufcap) {
            size_t newcap = bufcap == 0 ? 1024 : bufcap * 2;
            while (newcap < buflen + linelen + 1) newcap *= 2;
            buffer = (char*)realloc(buffer, newcap);
            bufcap = newcap;
        }
        memcpy(buffer + buflen, line, linelen);
        buflen += linelen;
    }

    if (hist) fclose(hist);
    free(buffer);
    return 0;
}
