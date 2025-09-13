#include "parser.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

/* very small global symbol table for LOAD_GLOBAL/STORE_GLOBAL */
static struct {
    char *names[VM_MAX_GLOBALS];
    int count;
} G = { {0}, 0 };

static int sym_index(const char *name) {
    for (int i = 0; i < G.count; ++i) {
        if (strcmp(G.names[i], name) == 0) return i;
    }
    if (G.count >= VM_MAX_GLOBALS) {
        fprintf(stderr, "Parser error: too many globals (max %d)\n", VM_MAX_GLOBALS);
        exit(1);
    }
    G.names[G.count] = strdup(name);
    return G.count++;
}

/* emit_expression compiles: string | number | true | false | identifier */
static int emit_expression(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    skip_spaces(src, len, pos);

    /* string */
    char *s = parse_string_literal_any_quote(src, len, pos);
    if (s) {
        int ci = bytecode_add_constant(bc, make_string(s));
        free(s);
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        return 1;
    }

    /* number */
    int ok = 0;
    size_t save = *pos;
    int64_t ival = parse_int_literal_value(src, len, pos, &ok);
    if (ok) {
        int ci = bytecode_add_constant(bc, make_int(ival));
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        return 1;
    }
    *pos = save;

    /* identifier or keyword */
    char *name = NULL;
    if (read_identifier_into(src, len, pos, &name)) {
        if (strcmp(name, "true") == 0 || strcmp(name, "false") == 0) {
            int ci = bytecode_add_constant(bc, make_int(strcmp(name, "true") == 0 ? 1 : 0));
            free(name);
            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
            return 1;
        }
        int gi = sym_index(name);
        free(name);
        bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
        return 1;
    }

    return 0;
}

/*
 * Very small compiler:
 * - Optionally skips: fun <ident>() { ... } wrapper.
 * - Emits PRINT for print(expr) and supports assignments: ident = expr.
 * - Supports literals: strings, integers, booleans; identifiers as globals.
 * - Ends with HALT.
 */
static Bytecode *compile_minimal(const char *src, size_t len) {
    Bytecode *bc = bytecode_new();
    size_t pos = 0;

    skip_shebang_if_present(src, len, &pos);

    // Optionally consume a single function wrapper: fun <ident>() { ... }
    skip_comments(src, len, &pos);
    skip_ws(src, len, &pos);
    if (starts_with(src, len, pos, "fun")) {
        pos += 3;
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        skip_identifier(src, len, &pos);  // function name
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        (void)consume_char(src, len, &pos, '(');
        (void)consume_char(src, len, &pos, ')');
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        (void)consume_char(src, len, &pos, '{');  // tolerate braces
    }

    // Parse simple statements until EOF or closing brace
    while (pos < len) {
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        if (pos < len && src[pos] == '}') { pos++; break; }
        if (pos >= len) break;

        // read an identifier (for 'print' or assignment), or handle print directly
        size_t stmt_start = pos;
        char *name = NULL;
        if (read_identifier_into(src, len, &pos, &name)) {
            // print(expr)
            if (strcmp(name, "print") == 0) {
                free(name);
                skip_spaces(src, len, &pos);
                (void)consume_char(src, len, &pos, '(');
                if (emit_expression(bc, src, len, &pos)) {
                    (void)consume_char(src, len, &pos, ')');
                    bytecode_add_instruction(bc, OP_PRINT, 0);
                } else {
                    // nothing to print; still try to close ')'
                    (void)consume_char(src, len, &pos, ')');
                }
                continue;
            }

            // assignment: ident = expr
            int gi = sym_index(name);
            free(name);
            skip_spaces(src, len, &pos);
            if (pos < len && src[pos] == '=') {
                pos++; // '='
                if (emit_expression(bc, src, len, &pos)) {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }
                // end of statement (consume to end of line or semicolon-like chars)
            } else if (pos < len && src[pos] == '(') {
                // Call to non-builtin: not supported yet; consume a simple ()-arg for now
                pos++; // '('
                // best effort: parse a single expression and drop it
                (void)emit_expression(bc, src, len, &pos);
                (void)consume_char(src, len, &pos, ')');
                // No CALL opcode emitted yet
            } else {
                // bare identifier line -> ignore for now
            }
            continue;
        }

        // 'print' without having read identifier (unlikely) - fallback
        if (starts_with(src, len, pos, "print")) {
            pos += 5;
            skip_spaces(src, len, &pos);
            (void)consume_char(src, len, &pos, '(');
            if (emit_expression(bc, src, len, &pos)) {
                (void)consume_char(src, len, &pos, ')');
                bytecode_add_instruction(bc, OP_PRINT, 0);
            } else {
                (void)consume_char(src, len, &pos, ')');
            }
            continue;
        }

        // Unknown/unsupported token: advance to avoid infinite loop
        if (stmt_start == pos) pos++;
    }

    bytecode_add_instruction(bc, OP_HALT, 0);
    return bc;
}

Bytecode *parse_file_to_bytecode(const char *path) {
    size_t len = 0;
    char *src = read_file_all(path, &len);
    if (!src) {
        fprintf(stderr, "Error: cannot read file: %s\n", path);
        return NULL;
    }
    Bytecode *bc = compile_minimal(src, len);
    free(src);
    return bc;
}