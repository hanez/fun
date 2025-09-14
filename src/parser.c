#include "parser.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* ---- parser error state ---- */
static int g_has_error = 0;
static size_t g_err_pos = 0;
static char g_err_msg[256];

static void parser_fail(size_t pos, const char *fmt, ...) {
    g_has_error = 1;
    g_err_pos = pos;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_err_msg, sizeof(g_err_msg), fmt, ap);
    va_end(ap);
}

static void calc_line_col(const char *src, size_t len, size_t pos, int *out_line, int *out_col) {
    int line = 1, col = 1;
    size_t limit = pos < len ? pos : len;
    for (size_t i = 0; i < limit; ++i) {
        if (src[i] == '\n') { line++; col = 1; }
        else { col++; }
    }
    if (out_line) *out_line = line;
    if (out_col) *out_col = col;
}

/* --------------------------- */

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
        parser_fail(0, "Too many globals (max %d)", VM_MAX_GLOBALS);
        return 0;
    }
    G.names[G.count] = strdup(name);
    return G.count++;
}

/* forward declaration so helpers can recurse */
static int emit_expression(Bytecode *bc, const char *src, size_t len, size_t *pos);

/* primary: (expr) | string | number | true/false | identifier */
static int emit_primary(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    skip_spaces(src, len, pos);

    /* parenthesized */
    if (*pos < len && src[*pos] == '(') {
        (*pos)++; /* '(' */
        if (!emit_expression(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after '('");
            return 0;
        }
        if (!consume_char(src, len, pos, ')')) {
            parser_fail(*pos, "Expected ')'");
            return 0;
        }
        return 1;
    }

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

/* unary: '!' unary | '-' unary | primary */
static int emit_unary(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    skip_spaces(src, len, pos);
    if (*pos < len && src[*pos] == '!') {
        (*pos)++;
        if (!emit_unary(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after '!'");
            return 0;
        }
        bytecode_add_instruction(bc, OP_NOT, 0);
        return 1;
    }
    if (*pos < len && src[*pos] == '-') {
        (*pos)++;
        /* unary minus -> 0 - expr */
        int ci = bytecode_add_constant(bc, make_int(0));
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        if (!emit_unary(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after unary '-'");
            return 0;
        }
        bytecode_add_instruction(bc, OP_SUB, 0);
        return 1;
    }
    return emit_primary(bc, src, len, pos);
}

/* multiplicative: unary (('*' | '/' | '%') unary)* */
static int emit_multiplicative(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_unary(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos < len && src[*pos] == '*') {
            (*pos)++;
            if (!emit_unary(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '*'"); return 0; }
            bytecode_add_instruction(bc, OP_MUL, 0);
            continue;
        }
        if (*pos < len && src[*pos] == '/') {
            (*pos)++;
            if (!emit_unary(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '/'"); return 0; }
            bytecode_add_instruction(bc, OP_DIV, 0);
            continue;
        }
        if (*pos < len && src[*pos] == '%') {
            (*pos)++;
            if (!emit_unary(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '%'"); return 0; }
            bytecode_add_instruction(bc, OP_MOD, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* additive: multiplicative (('+' | '-') multiplicative)* */
static int emit_additive(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_multiplicative(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos < len && src[*pos] == '+') {
            (*pos)++;
            if (!emit_multiplicative(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '+'"); return 0; }
            bytecode_add_instruction(bc, OP_ADD, 0);
            continue;
        }
        if (*pos < len && src[*pos] == '-') {
            (*pos)++;
            if (!emit_multiplicative(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '-'"); return 0; }
            bytecode_add_instruction(bc, OP_SUB, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* relational: additive (('<' | '<=' | '>' | '>=') additive)* */
static int emit_relational(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_additive(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos + 1 < len && src[*pos] == '<' && src[*pos + 1] == '=') {
            *pos += 2;
            if (!emit_additive(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '<='"); return 0; }
            bytecode_add_instruction(bc, OP_LTE, 0);
            continue;
        }
        if (*pos + 1 < len && src[*pos] == '>' && src[*pos + 1] == '=') {
            *pos += 2;
            if (!emit_additive(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '>='"); return 0; }
            bytecode_add_instruction(bc, OP_GTE, 0);
            continue;
        }
        if (*pos < len && src[*pos] == '<') {
            (*pos)++;
            if (!emit_additive(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '<'"); return 0; }
            bytecode_add_instruction(bc, OP_LT, 0);
            continue;
        }
        if (*pos < len && src[*pos] == '>') {
            (*pos)++;
            if (!emit_additive(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '>'"); return 0; }
            bytecode_add_instruction(bc, OP_GT, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* equality: relational (('==' | '!=') relational)* */
static int emit_equality(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_relational(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos + 1 < len && src[*pos] == '=' && src[*pos + 1] == '=') {
            *pos += 2;
            if (!emit_relational(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '=='"); return 0; }
            bytecode_add_instruction(bc, OP_EQ, 0);
            continue;
        }
        if (*pos + 1 < len && src[*pos] == '!' && src[*pos + 1] == '=') {
            *pos += 2;
            if (!emit_relational(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '!='"); return 0; }
            bytecode_add_instruction(bc, OP_NEQ, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* logical AND: equality ( '&&' equality )* */
static int emit_and_expr(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_equality(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos + 1 < len && src[*pos] == '&' && src[*pos + 1] == '&') {
            *pos += 2;
            if (!emit_equality(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '&&'"); return 0; }
            bytecode_add_instruction(bc, OP_AND, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* logical OR: and_expr ( '||' and_expr )* */
static int emit_or_expr(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    if (!emit_and_expr(bc, src, len, pos)) return 0;
    for (;;) {
        skip_spaces(src, len, pos);
        if (*pos + 1 < len && src[*pos] == '|' && src[*pos + 1] == '|') {
            *pos += 2;
            if (!emit_and_expr(bc, src, len, pos)) { parser_fail(*pos, "Expected expression after '||'"); return 0; }
            bytecode_add_instruction(bc, OP_OR, 0);
            continue;
        }
        break;
    }
    return 1;
}

/* top-level expression */
static int emit_expression(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    return emit_or_expr(bc, src, len, pos);
}

/*
 * Indentation-aware compiler (2 spaces):
 * - Optional shebang and a single fun <ident>() { ... } wrapper.
 * - Statements supported: print(expr), ident = expr, and if <expr> with an indented block.
 * - Literals: strings, integers, booleans; identifiers are globals.
 * - Ends with HALT.
 */

/* line/indent utilities */
static void skip_to_eol(const char *src, size_t len, size_t *pos) {
    while (*pos < len && src[*pos] != '\n') (*pos)++;
    if (*pos < len && src[*pos] == '\n') (*pos)++;
}

static int read_line_start(const char *src, size_t len, size_t *pos, int *out_indent) {
    while (*pos < len) {
        size_t p = *pos;
        int spaces = 0;
        while (p < len && src[p] == ' ') { spaces++; p++; }
        if (p < len && src[p] == '\t') {
            parser_fail(p, "Tabs are forbidden for indentation");
            return 0;
        }
        if (p >= len) { *pos = p; return 0; }
        if (src[p] == '\n') { /* empty line */ p++; *pos = p; continue; }

        /* comment-only line? */
        if (p + 1 < len && src[p] == '/' && src[p + 1] == '/') {
            /* skip entire line */
            while (p < len && src[p] != '\n') p++;
            if (p < len && src[p] == '\n') p++;
            *pos = p;
            continue;
        }

        if (spaces % 2 != 0) {
            parser_fail(p, "Indentation must be multiples of two spaces");
            return 0;
        }
        *out_indent = spaces / 2;
        *pos = p; /* point to first code char */
        return 1;
    }
    return 0;
}

/* forward decl */
static void parse_block(Bytecode *bc, const char *src, size_t len, size_t *pos, int current_indent);

/* parse and emit a single simple (non-if) statement on the current line */
static void parse_simple_statement(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    size_t local_pos = *pos;
    char *name = NULL;
    if (read_identifier_into(src, len, &local_pos, &name)) {
        /* typed declarations: number|string|boolean <ident> (= expr)? */
        if (strcmp(name, "number") == 0 || strcmp(name, "string") == 0 || strcmp(name, "boolean") == 0) {
            int is_number = (strcmp(name, "number") == 0);
            int is_string = (strcmp(name, "string") == 0);
            int is_boolean = (strcmp(name, "boolean") == 0);
            free(name);

            /* read variable name */
            char *varname = NULL;
            skip_spaces(src, len, &local_pos);
            if (!read_identifier_into(src, len, &local_pos, &varname)) {
                parser_fail(local_pos, "Expected identifier after type declaration");
                return;
            }
            int gi = sym_index(varname);
            free(varname);

            skip_spaces(src, len, &local_pos);
            if (local_pos < len && src[local_pos] == '=') {
                local_pos++; /* '=' */
                if (emit_expression(bc, src, len, &local_pos)) {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                } else {
                    parser_fail(local_pos, "Expected initializer expression after '='");
                    return;
                }
            } else {
                /* default initialize if no '=' given */
                int ci = -1;
                if (is_number || is_boolean) {
                    ci = bytecode_add_constant(bc, make_int(is_boolean ? 0 : 0));
                } else if (is_string) {
                    ci = bytecode_add_constant(bc, make_string(""));
                }
                if (ci >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        if (strcmp(name, "print") == 0) {
            free(name);
            skip_spaces(src, len, &local_pos);
            (void)consume_char(src, len, &local_pos, '(');
            if (emit_expression(bc, src, len, &local_pos)) {
                (void)consume_char(src, len, &local_pos, ')');
                bytecode_add_instruction(bc, OP_PRINT, 0);
            } else {
                (void)consume_char(src, len, &local_pos, ')');
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* assignment or simple call */
        int gi = sym_index(name);
        free(name);
        skip_spaces(src, len, &local_pos);
        if (local_pos < len && src[local_pos] == '=') {
            local_pos++; /* '=' */
            if (emit_expression(bc, src, len, &local_pos)) {
                bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        } else if (local_pos < len && src[local_pos] == '(') {
            /* simple call form (argument ignored/emitted but no CALL yet) */
            local_pos++; /* '(' */
            (void)emit_expression(bc, src, len, &local_pos);
            (void)consume_char(src, len, &local_pos, ')');
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        } else {
            /* bare identifier line */
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }
    }

    /* fallback: print(expr) without identifier read (unlikely) */
    if (starts_with(src, len, *pos, "print")) {
        *pos += 5;
        skip_spaces(src, len, pos);
        (void)consume_char(src, len, pos, '(');
        if (emit_expression(bc, src, len, pos)) {
            (void)consume_char(src, len, pos, ')');
            bytecode_add_instruction(bc, OP_PRINT, 0);
        } else {
            (void)consume_char(src, len, pos, ')');
        }
        skip_to_eol(src, len, pos);
        return;
    }

    /* unknown token: report error */
    parser_fail(*pos, "Unknown token at start of statement");
}

/* parse a block with lines at indentation >= current_indent; stop at dedent */
static void parse_block(Bytecode *bc, const char *src, size_t len, size_t *pos, int current_indent) {
    while (*pos < len) {
        if (g_has_error) return;
        size_t line_start = *pos;
        int indent = 0;
        if (!read_line_start(src, len, pos, &indent)) {
            /* EOF or error */
            return;
        }
        if (indent < current_indent) {
            /* dedent -> let caller handle this line */
            *pos = line_start;
            return;
        }
        if (indent > current_indent) {
            /* nested block without a header (tolerate by parsing it and continuing) */
            parse_block(bc, src, len, pos, indent);
            continue;
        }

        /* at same indent -> parse statement */
        if (starts_with(src, len, *pos, "if")) {
            int end_jumps[64];
            int end_count = 0;

            for (;;) {
                /* consume 'if' or 'else if' condition */
                if (starts_with(src, len, *pos, "if")) {
                    *pos += 2;
                } else {
                    /* for 'else if' we arrive here with *pos already after 'if' */
                }

                /* require at least one space before condition if present */
                skip_spaces(src, len, pos);
                if (!emit_expression(bc, src, len, pos)) {
                    /* no condition -> treat as false */
                    int ci = bytecode_add_constant(bc, make_int(0));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                }
                /* end of condition: ignore any trailing until EOL */
                skip_to_eol(src, len, pos);

                /* conditional jump over this clause's block */
                int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

                /* parse nested block if next line is indented */
                int next_indent = 0;
                size_t after_hdr = *pos;
                if (read_line_start(src, len, pos, &next_indent)) {
                    if (next_indent > current_indent) {
                        /* parse body at increased indent */
                        parse_block(bc, src, len, pos, next_indent);
                    } else {
                        /* empty body; keep *pos at start of that line */
                    }
                } else {
                    /* EOF -> empty body */
                }

                /* after body, unconditionally jump to end of the whole chain */
                int jmp_end = bytecode_add_instruction(bc, OP_JUMP, 0);
                if (end_count < (int)(sizeof(end_jumps) / sizeof(end_jumps[0]))) {
                    end_jumps[end_count++] = jmp_end;
                } else {
                    parser_fail(*pos, "Too many chained else/if clauses");
                    return;
                }

                /* patch false-jump target to start of next clause (or fallthrough) */
                bytecode_set_operand(bc, jmp_false, bc->instr_count);

                /* look for else or else if at the same indentation */
                size_t look = *pos;
                int look_indent = 0;
                if (!read_line_start(src, len, &look, &look_indent)) {
                    /* EOF: break and patch end jumps */
                    break;
                }
                if (look_indent != current_indent) {
                    /* dedent or deeper indent means no 'else' clause here */
                    break;
                }
                if (starts_with(src, len, look, "else")) {
                    /* consume 'else' */
                    *pos = look + 4;
                    skip_spaces(src, len, pos);

                    if (starts_with(src, len, *pos, "if")) {
                        /* else if -> consume 'if' token and continue loop to parse condition */
                        *pos += 2;
                        continue;
                    } else {
                        /* plain else: parse its block and finish the chain */
                        skip_to_eol(src, len, pos);
                        int else_indent = 0;
                        if (read_line_start(src, len, pos, &else_indent) && else_indent > current_indent) {
                            parse_block(bc, src, len, pos, else_indent);
                        } else {
                            /* empty else-body */
                        }
                        /* end of chain after else */
                        break;
                    }
                } else {
                    /* next line is not an else/else if */
                    break;
                }
            }

            /* patch all end-of-clause jumps to the end of the chain */
            for (int i = 0; i < end_count; ++i) {
                bytecode_set_operand(bc, end_jumps[i], bc->instr_count);
            }
            continue;
        }

        /* otherwise: simple statement on this line */
        parse_simple_statement(bc, src, len, pos);
    }
}

static Bytecode *compile_minimal(const char *src, size_t len) {
    Bytecode *bc = bytecode_new();
    size_t pos = 0;

    skip_shebang_if_present(src, len, &pos);

    /* Optional: consume a single function wrapper line: fun <ident>() { ... } */
    skip_comments(src, len, &pos);
    skip_ws(src, len, &pos);
    if (starts_with(src, len, pos, "fun")) {
        pos += 3;
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        skip_identifier(src, len, &pos);  /* name */
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        (void)consume_char(src, len, &pos, '(');
        (void)consume_char(src, len, &pos, ')');
        skip_comments(src, len, &pos);
        skip_ws(src, len, &pos);
        (void)consume_char(src, len, &pos, '{');  /* tolerate */
        /* move to next line start after wrapper */
        skip_to_eol(src, len, &pos);
    }

    /* parse the top-level block at indent 0 */
    parse_block(bc, src, len, &pos, 0);

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
    /* reset error state */
    g_has_error = 0;
    g_err_pos = 0;
    g_err_msg[0] = '\0';

    Bytecode *bc = compile_minimal(src, len);

    if (g_has_error) {
        int line = 1, col = 1;
        calc_line_col(src, len, g_err_pos, &line, &col);
        fprintf(stderr, "Parse error %s:%d:%d: %s\n", path ? path : "<input>", line, col, g_err_msg);
        if (bc) bytecode_free(bc);
        free(src);
        return NULL;
    }

    free(src);
    return bc;
}