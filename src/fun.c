#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

/* Detect if current buffer looks incomplete:
 * - Unclosed quotes (' or ") with escapes
 * - Unclosed block comment /* ... *\/
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
    printf("  :clear                 Clear current input buffer\n");
    printf("  :print                 Show current buffer\n");
    printf("  :run                   Execute current buffer immediately\n");
    printf("  :save <file>           Save current buffer to file\n");
    printf("  :load <file>           Load file into buffer (does not run)\n");
    printf("  :history [N]           Show last N lines of history (default 50)\n");
    printf("  :time on|off|toggle    Toggle/enable/disable timing\n");
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
    printf("Fun REPL. Type code and press Enter. Submit an empty line to run.\n");
    printf("Commands: :help, :reset, :dump, :quit\n");

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
        hist = fopen(hist_path, "a+");
    } else {
        snprintf(hist_path, sizeof(hist_path), ".fun_history");
        hist = fopen(hist_path, "a+");
    }

    for (;;) {
        fputs(buflen == 0 ? "fun> " : "... ", stdout);
        fflush(stdout);

        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) {
            puts("");
            break; // EOF
        }

        /* Command handling (available anytime) */
        if (line[0] == ':') {
            /* extract command and argument */
            char cmd[64] = {0};
            char arg[1024] = {0};
            sscanf(line, ":%63s %1023[^\n]", cmd, arg);

            if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0 || strcmp(cmd, "exit") == 0) {
                break;
            } else if (strcmp(cmd, "help") == 0) {
                show_repl_help();
                continue;
            } else if (strcmp(cmd, "reset") == 0) {
                vm_reset(&vm);
                printf("VM state reset.\n");
                continue;
            } else if (strcmp(cmd, "dump") == 0 || strcmp(cmd, "globals") == 0) {
                vm_dump_globals(&vm);
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
            } else if (strcmp(cmd, "run") == 0) {
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
                    } else {
                        printf("Parse error.\n");
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

            /* If buffer looks incomplete, keep reading */
            if (buffer_looks_incomplete(buffer)) {
                printf("(incomplete, continue typing)\n");
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
                } else {
                    printf("Parse error.\n");
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

