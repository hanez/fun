/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file parser.c
 * @brief Implements the Fun language parser that converts source code to bytecode.
 *
 * This file contains the main parsing logic for the Fun programming language.
 * It handles converting .fun source files into executable bytecode for the VM.
 *
 * Key Features:
 * - Handles shebang lines
 * - Skips whitespace and comments
 * - Parses string literals with both single and double quotes
 * - Supports basic function definitions
 * - Compiles print statements
 * - Generates bytecode with proper constants and instructions
 *
 * Functions:
 * - parse_file_to_bytecode(): Main entry point for file parsing
 * - parse_string_to_bytecode(): Parses code from string buffers
 * - parser_last_error(): Retrieves parsing errors
 *
 * Error Handling:
 * - Returns NULL on parse errors
 * - Tracks error messages and positions
 * - Validates syntax before bytecode generation
 *
 * Example:
 * Bytecode *bc = parse_file_to_bytecode("example.fun");
 * if (bc) {
 *     vm_run(&vm, bc);
 *     bytecode_free(bc);
 * }
 *
 * @author Johannes Findeisen
 * @date 2025-09-16
 */

#include "parser.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

/* ---- parser error state ---- */
static const char *g_current_source_path = NULL; /* for propagating filename into nested bytecodes */
static int g_has_error = 0;
static size_t g_err_pos = 0;
static char g_err_msg[256];
static int g_err_line = 0;
static int g_err_col  = 0;

/* ---- compiler-generated temporary counter ---- */
static int g_temp_counter = 0;

/* Declared type metadata encoding in types[]:
   0 = dynamic/untyped;
   positive/negative 8/16/32/64 = integers (negative means signed);
   TYPE_META_STRING/BOOLEAN/NIL mark non-integer enforced types;
   TYPE_META_CLASS marks class instances (Map with "__class"). */
#define TYPE_META_STRING  10001
#define TYPE_META_BOOLEAN 10002
#define TYPE_META_NIL     10003
#define TYPE_META_CLASS   10004
#define TYPE_META_FLOAT   10005
#define TYPE_META_ARRAY   10006

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

/* Namespace alias tracking (for include-as):
   We scan preprocessed source for lines starting with "// __ns_alias__: <name>"
   and treat dot-calls on those identifiers as plain function calls (no implicit 'this'). */
static char *g_ns_aliases[64];
static int   g_ns_alias_count = 0;

static void ns_aliases_reset(void) {
    for (int i = 0; i < g_ns_alias_count; ++i) {
        free(g_ns_aliases[i]);
        g_ns_aliases[i] = NULL;
    }
    g_ns_alias_count = 0;
}

static void ns_aliases_scan(const char *src, size_t len) {
    const char *marker = "// __ns_alias__: ";
    size_t mlen = strlen(marker);
    size_t i = 0;
    while (i < len) {
        /* find start of line */
        size_t ls = i;
        /* move to end of line first */
        while (i < len && src[i] != '\n') i++;
        size_t le = i;
        /* include trailing '\n' in next iteration */
        if (i < len && src[i] == '\n') i++;

        if (le - ls >= mlen && strncmp(src + ls, marker, mlen) == 0) {
            size_t p = ls + mlen;
            /* read identifier */
            size_t start = p;
            while (p < le && (src[p] == ' ' || src[p] == '\t')) p++;
            if (p < le && (isalpha((unsigned char)src[p]) || src[p] == '_')) {
                size_t q = p + 1;
                while (q < le && (isalnum((unsigned char)src[q]) || src[q] == '_')) q++;
                size_t n = q - p;
                if (n > 0 && g_ns_alias_count < (int)(sizeof(g_ns_aliases)/sizeof(g_ns_aliases[0]))) {
                    char *name = (char*)malloc(n + 1);
                    if (name) {
                        memcpy(name, src + p, n);
                        name[n] = '\0';
                        g_ns_aliases[g_ns_alias_count++] = name;
                    }
                }
            }
        }
    }
}

static int is_ns_alias(const char *name) {
    if (!name) return 0;
    for (int i = 0; i < g_ns_alias_count; ++i) {
        if (strcmp(g_ns_aliases[i], name) == 0) return 1;
    }
    return 0;
}

#include "parser_utils.c"

/* very small global symbol table for LOAD_GLOBAL/STORE_GLOBAL */
static struct {
    char *names[MAX_GLOBALS];
    int types[MAX_GLOBALS];   /* 0=untyped/number default; else bit width: 8/16/32/64; negative for signed */
    int is_class[MAX_GLOBALS];/* 1 if this global name denotes a class factory */
    int count;
} G = { {0}, {0}, {0}, 0 };

static int sym_index(const char *name) {
    for (int i = 0; i < G.count; ++i) {
        if (strcmp(G.names[i], name) == 0) return i;
    }
    if (G.count >= MAX_GLOBALS) {
        parser_fail(0, "Too many globals (max %d)", MAX_GLOBALS);
        return 0;
    }
    G.names[G.count] = strdup(name);
    G.types[G.count] = 0;   /* default: untyped */
    G.is_class[G.count] = 0;/* default: not a class */
    return G.count++;
}

/* ---- locals environment for functions ---- */
typedef struct {
    char *names[MAX_FRAME_LOCALS];
    int  types[MAX_FRAME_LOCALS]; /* 0=untyped; else bit width: 8/16/32/64 */
    int count;
} LocalEnv;

static LocalEnv *g_locals = NULL;

/* loop context for break/continue patching */
typedef struct LoopCtx {
    int break_jumps[64];
    int break_count;
    int continue_jumps[64];
    int cont_count;
    struct LoopCtx *prev;
} LoopCtx;

static LoopCtx *g_loop_ctx = NULL;

static int local_find(const char *name) {
    if (!g_locals) return -1;
    for (int i = 0; i < g_locals->count; ++i) {
        if (strcmp(g_locals->names[i], name) == 0) return i;
    }
    return -1;
}

static int local_add(const char *name) {
    if (!g_locals) return -1;
    if (g_locals->count >= MAX_FRAME_LOCALS) {
        parser_fail(0, "Too many local variables/parameters (max %d)", MAX_FRAME_LOCALS);
        return -1;
    }
    int idx = g_locals->count++;
    g_locals->names[idx] = strdup(name);
    return idx;
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
        /* postfix indexing or slice */
        for (;;) {
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '[') {
                (*pos)++;
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); return 0; }
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ':') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    size_t savep4 = *pos;
                    if (!emit_expression(bc, src, len, pos)) {
                        *pos = savep4;
                        int ci4 = bytecode_add_constant(bc, make_int(-1));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ci4);
                    }
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); return 0; }
                    bytecode_add_instruction(bc, OP_SLICE, 0);
                    continue;
                } else {
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); return 0; }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
                }
            }
            break;
        }
        return 1;
    }

    /* string */
    char *s = parse_string_literal_any_quote(src, len, pos);
    if (s) {
        int ci = bytecode_add_constant(bc, make_string(s));
        free(s);
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        /* postfix indexing or slice */
        for (;;) {
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '[') {
                (*pos)++;
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); return 0; }
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ':') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    /* end is optional; if missing, use -1 (till end) */
                    int has_end = 0;
                    size_t savep = *pos;
                    if (emit_expression(bc, src, len, pos)) {
                        has_end = 1;
                    } else {
                        *pos = savep;
                        int ci = bytecode_add_constant(bc, make_int(-1));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                    }
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); return 0; }
                    bytecode_add_instruction(bc, OP_SLICE, 0);
                    continue;
                } else {
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); return 0; }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
                }
            }
            break;
        }
        return 1;
    }

    /* array literal: [expr, expr, ...] */
    skip_spaces(src, len, pos);
    if (*pos < len && src[*pos] == '[') {
        (*pos)++; /* '[' */
        int count = 0;
        skip_spaces(src, len, pos);
        if (*pos < len && src[*pos] != ']') {
            for (;;) {
                if (!emit_expression(bc, src, len, pos)) {
                    parser_fail(*pos, "Expected expression in array literal");
                    return 0;
                }
                count++;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); continue; }
                break;
            }
        }
        if (!consume_char(src, len, pos, ']')) {
            parser_fail(*pos, "Expected ']' to close array literal");
            return 0;
        }
        bytecode_add_instruction(bc, OP_MAKE_ARRAY, count);
        /* postfix indexing or slice */
        for (;;) {
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '[') {
                (*pos)++;
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); return 0; }
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ':') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    size_t savep3 = *pos;
                    if (!emit_expression(bc, src, len, pos)) {
                        *pos = savep3;
                        int ci3 = bytecode_add_constant(bc, make_int(-1));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ci3);
                    }
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); return 0; }
                    bytecode_add_instruction(bc, OP_SLICE, 0);
                    continue;
                } else {
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); return 0; }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
                }
            }
            break;
        }
        return 1;
    }

    /* map literal: { "key": expr, ... } */
    skip_spaces(src, len, pos);
    if (*pos < len && src[*pos] == '{') {
        (*pos)++; /* '{' */
        int pairs = 0;
        skip_spaces(src, len, pos);
        if (*pos < len && src[*pos] != '}') {
            for (;;) {
                /* key must be a string literal */
                char *k = parse_string_literal_any_quote(src, len, pos);
                if (!k) { parser_fail(*pos, "Expected string key in map literal"); return 0; }
                int kci = bytecode_add_constant(bc, make_string(k));
                free(k);
                bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                skip_spaces(src, len, pos);
                if (!consume_char(src, len, pos, ':')) { parser_fail(*pos, "Expected ':' after map key"); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected value expression in map literal"); return 0; }
                pairs++;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); continue; }
                break;
            }
        }
        if (!consume_char(src, len, pos, '}')) {
            parser_fail(*pos, "Expected '}' to close map literal");
            return 0;
        }
        bytecode_add_instruction(bc, OP_MAKE_MAP, pairs);
        return 1;
    }

    /* number (prefer float first to consume cases like 1.23 or 1e2) */
    int ok = 0;
    size_t save = *pos;
    /* float literal */
    double fval = parse_float_literal_value(src, len, pos, &ok);
    if (ok) {
        int ci = bytecode_add_constant(bc, make_float(fval));
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        /* postfix indexing or slice (not typical for floats but keep consistency) */
        for (;;) {
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '[') {
                (*pos)++;
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); return 0; }
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ':') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    size_t savep2 = *pos;
                    if (!emit_expression(bc, src, len, pos)) {
                        *pos = savep2;
                        int ci2 = bytecode_add_constant(bc, make_int(-1));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ci2);
                    }
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); return 0; }
                    bytecode_add_instruction(bc, OP_SLICE, 0);
                    continue;
                } else {
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); return 0; }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
                }
            }
            break;
        }
        return 1;
    }
    *pos = save;
    /* integer literal */
    int64_t ival = parse_int_literal_value(src, len, pos, &ok);
    if (ok) {
        int ci = bytecode_add_constant(bc, make_int(ival));
        bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
        /* postfix indexing or slice */
        for (;;) {
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '[') {
                (*pos)++;
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); return 0; }
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ':') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    int has_end = 0;
                    size_t savep2 = *pos;
                    if (emit_expression(bc, src, len, pos)) {
                        has_end = 1;
                    } else {
                        *pos = savep2;
                        int ci2 = bytecode_add_constant(bc, make_int(-1));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ci2);
                    }
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); return 0; }
                    bytecode_add_instruction(bc, OP_SLICE, 0);
                    continue;
                } else {
                    if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); return 0; }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
                }
            }
            break;
        }
        return 1;
    }

    /* identifier or keyword */
    char *name = NULL;
    if (read_identifier_into(src, len, pos, &name)) {
        if (strcmp(name, "true") == 0 || strcmp(name, "false") == 0) {
            int ci = bytecode_add_constant(bc, make_bool(strcmp(name, "true") == 0 ? 1 : 0));
            free(name);
            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
            return 1;
        }

        /* call or variable load (locals preferred) */
        skip_spaces(src, len, pos);
        int local_idx = local_find(name);
        int is_call = (*pos < len && src[*pos] == '(');

        if (is_call) {
            /* builtins */
            if (strcmp(name, "len") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "len expects 1 argument"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after len arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "push") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "push expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "push expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "push expects value"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after push args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PUSH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pop") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pop expects array"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pop arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_APOP, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "set") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "set expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "set expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "set expects index"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "set expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "set expects value"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after set args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SET, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "insert") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "insert expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "insert expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "insert expects index"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "insert expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "insert expects value"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after insert args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INSERT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "remove") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "remove expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "remove expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "remove expects index"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after remove args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_REMOVE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "to_number") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "to_number expects 1 argument"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after to_number arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TO_NUMBER, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "to_string") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "to_string expects 1 argument"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after to_string arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TO_STRING, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "cast") == 0) {
                (*pos)++; /* '(' */
                /* cast(value, typeName) */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "cast expects (value, typeName)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "cast expects (value, typeName)"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CAST, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "typeof") == 0) {
                (*pos)++; /* '(' */
                /* Special handling for typeof(<identifier>) to return declared subtype for integers */
                size_t peek = *pos;
                char *vname = NULL;
                int handled = 0;
                if (read_identifier_into(src, len, &peek, &vname)) {
                    skip_spaces(src, len, &peek);
                    if (peek < len && src[peek] == ')') {
                        int meta = 0;
                        int lidx = local_find(vname);
                        if (lidx >= 0) {
                            meta = g_locals->types[lidx];
                        } else {
                            int gi = sym_index(vname);
                            if (gi >= 0) meta = G.types[gi];
                        }

                        if (meta != 0 && meta != TYPE_META_STRING && meta != TYPE_META_BOOLEAN && meta != TYPE_META_NIL && meta != TYPE_META_CLASS && meta != TYPE_META_FLOAT) {
                            /* Integer subtype: ±bits */
                            int abs_bits = meta < 0 ? -meta : meta;
                            const char *tname = (meta < 0)
                                ? (abs_bits==64? "Sint64" : (abs_bits==32? "Sint32" : (abs_bits==16? "Sint16" : "Sint8")))
                                : (abs_bits==64? "Uint64" : (abs_bits==32? "Uint32" : (abs_bits==16? "Uint16" : "Uint8")));
                            int ci = bytecode_add_constant(bc, make_string(tname));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                            *pos = peek + 1; /* consume name and ')' */
                            handled = 1;
                        }
                        free(vname);
                    } else {
                        free(vname);
                    }
                }

                if (!handled) {
                    /* General case: typeof(expression)
                       If the value is a Map with "__class" key, return that string; else return base typeof.
                    */
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "typeof expects 1 argument"); free(name); return 0; }
                    if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after typeof arg"); free(name); return 0; }

                    /* [v] */
                    bytecode_add_instruction(bc, OP_DUP, 0);            /* [v, v] */
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);        /* [v, tname] */
                    {
                        int ciMap = bytecode_add_constant(bc, make_string("Map"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMap); /* [v, tname, "Map"] */
                    }
                    bytecode_add_instruction(bc, OP_EQ, 0);            /* [v, isMap] */
                    int j_if_not_map = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

                    /* Map branch: [v] */
                    bytecode_add_instruction(bc, OP_DUP, 0);           /* [v, v] */
                    {
                        int kci = bytecode_add_constant(bc, make_string("__class"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci); /* [v, v, "__class"] */
                    }
                    bytecode_add_instruction(bc, OP_HAS_KEY, 0);       /* [v, has] */
                    int j_no_meta = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* has __class: try toString() -> return its result */
                    bytecode_add_instruction(bc, OP_DUP, 0);           /* [v, v] */
                    {
                        int kcits = bytecode_add_constant(bc, make_string("toString"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kcits); /* [v, v, "toString"] */
                    }
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);     /* [v, func] */
                    bytecode_add_instruction(bc, OP_SWAP, 0);          /* [func, v] */
                    bytecode_add_instruction(bc, OP_CALL, 1);          /* [string] */
                    int j_end = bytecode_add_instruction(bc, OP_JUMP, 0);

                    /* no meta: drop v and return "Map" */
                    bytecode_set_operand(bc, j_no_meta, bc->instr_count);
                    bytecode_add_instruction(bc, OP_POP, 0);           /* [] */
                    {
                        int ciMap2 = bytecode_add_constant(bc, make_string("Map"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMap2); /* ["Map"] */
                    }
                    int j_end2 = bytecode_add_instruction(bc, OP_JUMP, 0);
                    int after_map = bc->instr_count;

                    /* not map: compute typeof(v) */
                    bytecode_set_operand(bc, j_if_not_map, after_map);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);        /* [tname] */

                    /* end */
                    bytecode_set_operand(bc, j_end, bc->instr_count);
                    bytecode_set_operand(bc, j_end2, bc->instr_count);
                }

                free(name);
                return 1;
            }
            if (strcmp(name, "keys") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "keys expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_KEYS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "values") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "values expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_VALUES, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "has") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "has expects (map, key)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "has expects (map, key)"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_HAS_KEY, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "read_file") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "read_file expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_READ_FILE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "write_file") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "write_file expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "write_file expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_WRITE_FILE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "input") == 0) {
                (*pos)++; /* '(' */
                int hasPrompt = 0;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] != ')') {
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "input expects 0 or 1 argument"); free(name); return 0; }
                    hasPrompt = 1;
                }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after input arg(s)"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INPUT_LINE, hasPrompt ? 1 : 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "input_hidden") == 0) {
                (*pos)++; /* '(' */
                int hasPrompt = 0;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] != ')') {
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "input_hidden expects 0 or 1 argument"); free(name); return 0; }
                    hasPrompt = 1;
                }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after input_hidden arg(s)"); free(name); return 0; }
                /* operand bit0 = hasPrompt, bit1 = hidden */
                bytecode_add_instruction(bc, OP_INPUT_LINE, (hasPrompt ? 1 : 0) | 2);
                free(name);
                return 1;
            }
            if (strcmp(name, "proc_run") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "proc_run expects 1 argument (command string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after proc_run arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PROC_RUN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "system") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "system expects 1 argument (command string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after system arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PROC_SYSTEM, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "time_now_ms") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "time_now_ms expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TIME_NOW_MS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "clock_mono_ms") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "clock_mono_ms expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CLOCK_MONO_MS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "date_format") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "date_format expects (ms:int, fmt:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "date_format expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "date_format expects (ms:int, fmt:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after date_format args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_DATE_FORMAT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "env") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "env expects 1 argument"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after env arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ENV, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "env_all") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "env_all expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ENV_ALL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "fun_version") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "fun_version expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_FUN_VERSION, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "rust_hello") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "rust_hello expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RUST_HELLO, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "rust_hello_args") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "rust_hello_args expects (message:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after rust_hello_args arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RUST_HELLO_ARGS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "rust_get_sp") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "rust_get_sp expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RUST_GET_SP, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "rust_set_exit") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "rust_set_exit expects (code:int)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after rust_set_exit arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RUST_SET_EXIT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "os_list_dir") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "os_list_dir expects (path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after os_list_dir arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_OS_LIST_DIR, 0);
                free(name);
                return 1;
            }
            /* JSON builtins */
            if (strcmp(name, "json_parse") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_parse expects (text)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after json_parse arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_JSON_PARSE, 0);
                free(name);
                return 1;
            }
            /* XML builtins (minimal) */
            if (strcmp(name, "xml_parse") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "xml_parse expects (text)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after xml_parse arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_XML_PARSE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "xml_root") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "xml_root expects (doc_handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after xml_root arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_XML_ROOT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "xml_name") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "xml_name expects (node_handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after xml_name arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_XML_NAME, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "xml_text") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "xml_text expects (node_handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after xml_text arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_XML_TEXT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "json_stringify") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_stringify expects (value, pretty)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "json_stringify expects (value, pretty)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_stringify expects (value, pretty)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after json_stringify args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_JSON_STRINGIFY, 0);
                free(name);
                return 1;
            }
            /* Tk (GUI) builtins (no raw Tcl exposed) */
            if (strcmp(name, "tk_loop") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "tk_loop expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_LOOP, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_title") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_title expects (title:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_title arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_WM_TITLE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_label") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_label expects (id:string, text:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tk_label expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_label expects (id:string, text:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_label args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_LABEL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_button") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_button expects (id:string, text:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tk_button expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_button expects (id:string, text:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_button args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_BUTTON, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_pack") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_pack expects (id:string)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_pack arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_PACK, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_bind") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_bind expects (id, event, cmd)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tk_bind expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_bind expects 3 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tk_bind expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_bind expects 3 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_bind args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_BIND, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_eval") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tk_eval expects (script)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tk_eval arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_EVAL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tk_result") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "tk_result expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TK_RESULT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "json_from_file") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_from_file expects (path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after json_from_file arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_JSON_FROM_FILE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "json_to_file") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_to_file expects (path, value, pretty)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "json_to_file expects (path, value, pretty)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_to_file expects (path, value, pretty)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "json_to_file expects (path, value, pretty)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "json_to_file expects (path, value, pretty)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after json_to_file args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_JSON_TO_FILE, 0);
                free(name);
                return 1;
            }
            /* INI (iniparser 4.2.6) builtins */
            if (strcmp(name, "ini_load") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_load expects (path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_load arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_LOAD, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_free") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_free expects (handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_free arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_FREE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_get_string") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_string expects (handle, section, key, default)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_string expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_get_string args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_GET_STRING, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_get_int") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_int expects (handle, section, key, default)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_int expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_get_int args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_GET_INT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_get_double") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_double expects (handle, section, key, default)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_double expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_get_double args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_GET_DOUBLE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_get_bool") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_bool expects (handle, section, key, default)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_get_bool expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_get_bool args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_GET_BOOL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_set") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_set expects (handle, section, key, value)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_set expects 4 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_set args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_SET, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_unset") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_unset expects (handle, section, key)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_unset expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_unset expects 3 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_unset expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_unset expects 3 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_unset args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_UNSET, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ini_save") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_save expects (handle, path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "ini_save expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "ini_save expects 2 args"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after ini_save args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INI_SAVE, 0);
                free(name);
                return 1;
            }
            /* CURL builtins (minimal interface like JSON) */
            if (strcmp(name, "curl_get") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "curl_get expects (url)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after curl_get arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CURL_GET, 0);
                free(name);
                return 1;
            }
            /* SQLite builtins */
            if (strcmp(name, "sqlite_open") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_open expects (path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sqlite_open arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SQLITE_OPEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sqlite_close") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_close expects (handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sqlite_close arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SQLITE_CLOSE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sqlite_exec") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_exec expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "sqlite_exec expects (handle, sql)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_exec expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sqlite_exec args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SQLITE_EXEC, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sqlite_query") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_query expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "sqlite_query expects (handle, sql)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sqlite_query expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sqlite_query args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SQLITE_QUERY, 0);
                free(name);
                return 1;
            }
            /* libsql builtins (independent extension) */
            if (strcmp(name, "libsql_open") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_open expects (url_or_path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after libsql_open arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LIBSQL_OPEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "libsql_close") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_close expects (handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after libsql_close arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LIBSQL_CLOSE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "libsql_exec") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_exec expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "libsql_exec expects (handle, sql)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_exec expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after libsql_exec args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LIBSQL_EXEC, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "libsql_query") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_query expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "libsql_query expects (handle, sql)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "libsql_query expects (handle, sql)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after libsql_query args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LIBSQL_QUERY, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "curl_post") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "curl_post expects (url, body)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "curl_post expects (url, body)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "curl_post expects (url, body)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after curl_post args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CURL_POST, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "curl_download") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "curl_download expects (url, path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "curl_download expects (url, path)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "curl_download expects (url, path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after curl_download args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CURL_DOWNLOAD, 0);
                free(name);
                return 1;
            }
            /* PCSC builtins */
            if (strcmp(name, "pcsc_establish") == 0) {
                (*pos)++; /* '(' */
                /* no args */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "pcsc_establish expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_ESTABLISH, 0);
                free(name);
                return 1;
            }
            /* PCRE2 builtins */
            if (strcmp(name, "pcre2_test") == 0) {
                (*pos)++; /* '(' */
                /* (pattern, text, flags) */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_test expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_test expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_test expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_test expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_test expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcre2_test args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCRE2_TEST, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcre2_match") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_match expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_match expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_match expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_match expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_match expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcre2_match args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCRE2_MATCH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcre2_findall") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_findall expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_findall expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_findall expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcre2_findall expects (pattern, text, flags)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcre2_findall expects (pattern, text, flags)" ); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcre2_findall args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCRE2_FINDALL, 0);
                free(name);
                return 1;
            }
            /* Notcurses builtins (optional) */
            if (strcmp(name, "nc_init") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "nc_init expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_NC_INIT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "nc_shutdown") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "nc_shutdown expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_NC_SHUTDOWN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "nc_clear") == 0) {
                (*pos)++; /* '(' */
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "nc_clear expects ()"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_NC_CLEAR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "nc_draw_text") == 0) {
                (*pos)++; /* '(' */
                /* (y, x, text) -> push y, x, text, then opcode will pop text,x,y */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "nc_draw_text expects (y, x, text)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "nc_draw_text expects (y, x, text)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "nc_draw_text expects (y, x, text)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "nc_draw_text expects (y, x, text)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "nc_draw_text expects (y, x, text)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after nc_draw_text args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_NC_DRAW_TEXT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "nc_getch") == 0) {
                (*pos)++; /* '(' */
                /* (timeout_ms) */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "nc_getch expects (timeout_ms)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after nc_getch arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_NC_GETCH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcsc_release") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_release expects 1 argument (ctx)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcsc_release arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_RELEASE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcsc_list_readers") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_list_readers expects 1 argument (ctx)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcsc_list_readers arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_LIST_READERS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcsc_connect") == 0) {
                (*pos)++; /* '(' */
                /* ctx, reader */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_connect expects (ctx, reader)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcsc_connect expects (ctx, reader)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_connect expects (ctx, reader)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcsc_connect args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_CONNECT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcsc_disconnect") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_disconnect expects 1 argument (handle)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcsc_disconnect arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_DISCONNECT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pcsc_transmit") == 0) {
                (*pos)++; /* '(' */
                /* handle, bytes */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_transmit expects (handle, bytes)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "pcsc_transmit expects (handle, bytes)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pcsc_transmit expects (handle, bytes)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pcsc_transmit args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_PCSC_TRANSMIT, 0);
                free(name);
                return 1;
            }
            /* Socket builtins */
            if (strcmp(name, "tcp_listen") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tcp_listen expects (port, backlog)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tcp_listen expects (port, backlog)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tcp_listen expects (port, backlog)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tcp_listen args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_TCP_LISTEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tcp_accept") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tcp_accept expects (listen_fd)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tcp_accept arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_TCP_ACCEPT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tcp_connect") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tcp_connect expects (host, port)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "tcp_connect expects (host, port)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "tcp_connect expects (host, port)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after tcp_connect args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_TCP_CONNECT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sock_send") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sock_send expects (fd, data)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "sock_send expects (fd, data)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sock_send expects (fd, data)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sock_send args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_SEND, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sock_recv") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sock_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "sock_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sock_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sock_recv args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_RECV, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sock_close") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "sock_close expects (fd)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after sock_close arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_CLOSE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "unix_listen") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "unix_listen expects (path, backlog)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "unix_listen expects (path, backlog)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "unix_listen expects (path, backlog)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after unix_listen args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_UNIX_LISTEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "unix_connect") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "unix_connect expects (path)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after unix_connect arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SOCK_UNIX_CONNECT, 0);
                free(name);
                return 1;
            }
            /* Serial builtins */
            if (strcmp(name, "serial_open") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_open expects (path, baud)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "serial_open expects (path, baud)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_open expects (path, baud)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after serial_open args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SERIAL_OPEN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "serial_config") == 0) {
                (*pos)++; /* '(' */
                // fd, data_bits, parity, stop_bits, flow_control
                for (int i = 0; i < 5; ++i) {
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_config expects 5 arguments"); free(name); return 0; }
                    if (i < 4) {
                        if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "serial_config expects 5 arguments"); free(name); return 0; }
                    }
                }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after serial_config args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SERIAL_CONFIG, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "serial_send") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_send expects (fd, data)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "serial_send expects (fd, data)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_send expects (fd, data)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after serial_send args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SERIAL_SEND, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "serial_recv") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ',')) { parser_fail(*pos, "serial_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_recv expects (fd, maxlen)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after serial_recv args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SERIAL_RECV, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "serial_close") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "serial_close expects (fd)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after serial_close arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SERIAL_CLOSE, 0);
                free(name);
                return 1;
            }
            /* string ops */
            if (strcmp(name, "split") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "split expects string"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "split expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "split expects separator"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after split args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SPLIT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "join") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "join expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "join expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "join expects separator"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after join args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_JOIN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "substr") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "substr expects string"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "substr expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "substr expects start"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "substr expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "substr expects len"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after substr args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SUBSTR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "find") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "find expects haystack"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "find expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "find expects needle"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after find args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_FIND, 0);
                free(name);
                return 1;
            }
            /* regex ops */
            if (strcmp(name, "regex_match") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_match expects text"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "regex_match expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_match expects pattern"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after regex_match args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_REGEX_MATCH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "regex_search") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_search expects text"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "regex_search expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_search expects pattern"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after regex_search args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_REGEX_SEARCH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "regex_replace") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_replace expects text"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "regex_replace expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_replace expects pattern"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "regex_replace expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "regex_replace expects replacement"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after regex_replace args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_REGEX_REPLACE, 0);
                free(name);
                return 1;
            }
            /* array utils */
            if (strcmp(name, "contains") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "contains expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "contains expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "contains expects value"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after contains args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CONTAINS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "indexOf") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "indexOf expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "indexOf expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "indexOf expects value"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after indexOf args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_INDEX_OF, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "clear") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "clear expects array"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after clear arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CLEAR, 0);
                free(name);
                return 1;
            }
            /* iteration helpers */
            if (strcmp(name, "enumerate") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "enumerate expects array"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after enumerate arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ENUMERATE, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "map") == 0) {
                (*pos)++; /* '(' */
                /* arr */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "map expects (array, function)"); free(name); return 0; }
                /* store arr -> __map_arr */
                char tarr[64]; snprintf(tarr, sizeof(tarr), "__map_arr_%d", g_temp_counter++);
                int larr = -1, garr = -1;
                if (g_locals) { larr = local_add(tarr); bytecode_add_instruction(bc, OP_STORE_LOCAL, larr); }
                else { garr = sym_index(tarr); bytecode_add_instruction(bc, OP_STORE_GLOBAL, garr); }
                /* func */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "map expects (array, function)"); free(name); return 0; }
                char tfn[64]; snprintf(tfn, sizeof(tfn), "__map_fn_%d", g_temp_counter++);
                int lfn = -1, gfn = -1;
                if (g_locals) { lfn = local_add(tfn); bytecode_add_instruction(bc, OP_STORE_LOCAL, lfn); }
                else { gfn = sym_index(tfn); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gfn); }
                /* res array */
                bytecode_add_instruction(bc, OP_MAKE_ARRAY, 0);
                char tres[64]; snprintf(tres, sizeof(tres), "__map_res_%d", g_temp_counter++);
                int lres = -1, gres = -1;
                if (g_locals) { lres = local_add(tres); bytecode_add_instruction(bc, OP_STORE_LOCAL, lres); }
                else { gres = sym_index(tres); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gres); }
                /* i=0 */
                int c0 = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
                char ti[64]; snprintf(ti, sizeof(ti), "__map_i_%d", g_temp_counter++);
                int li = -1, gi = -1;
                if (g_locals) { li = local_add(ti); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { gi = sym_index(ti); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                /* loop start */
                int loop_start = bc->instr_count;
                /* i < len(arr) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); }
                bytecode_add_instruction(bc, OP_LEN, 0);
                bytecode_add_instruction(bc, OP_LT, 0);
                int jf = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                /* elem = arr[i] */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                /* call fn(elem) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lfn); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gfn); }
                /* reorder: we need fn below arg -> push fn first then arg already on stack? We currently have elem on stack; push fn now results top=fn. We want top args then function; OP_CALL expects fn then pops args? Our OP_CALL pops function after args; earlier compile of calls: they push function then args then OP_CALL. So we need to swap: push fn, then swap to make function below arg */
                bytecode_add_instruction(bc, OP_SWAP, 0);
                bytecode_add_instruction(bc, OP_CALL, 1);
                /* Append to result via indexed assignment: res[len(res)] = value */
                /* Store computed value to a temp */
                char tv[64]; snprintf(tv, sizeof(tv), "__map_v_%d", g_temp_counter++);
                int lv = -1, gv = -1;
                if (g_locals) { lv = local_add(tv); bytecode_add_instruction(bc, OP_STORE_LOCAL, lv); }
                else { gv = sym_index(tv); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gv); }

                /* Push array (for INDEX_SET we need stack: value, index, array; we will build array, index, then value) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }

                /* Compute index = len(res) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }
                bytecode_add_instruction(bc, OP_LEN, 0);

                /* Load value back on top */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lv); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gv); }

                /* Append via insert: res.insert(index=len(res), value) */
                bytecode_add_instruction(bc, OP_INSERT, 0);
                /* dApache-2.0ard returned new length */
                bytecode_add_instruction(bc, OP_POP, 0);

                /* i++ */
                int c1 = bytecode_add_constant(bc, make_int(1));
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_JUMP, loop_start);
                bytecode_set_operand(bc, jf, bc->instr_count);
                /* result value on stack */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }
                free(name);
                return 1;
            }
            if (strcmp(name, "filter") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "filter expects (array, function)"); free(name); return 0; }
                char tarr[64]; snprintf(tarr, sizeof(tarr), "__flt_arr_%d", g_temp_counter++);
                int larr = -1, garr = -1;
                if (g_locals) { larr = local_add(tarr); bytecode_add_instruction(bc, OP_STORE_LOCAL, larr); }
                else { garr = sym_index(tarr); bytecode_add_instruction(bc, OP_STORE_GLOBAL, garr); }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "filter expects (array, function)"); free(name); return 0; }
                char tfn[64]; snprintf(tfn, sizeof(tfn), "__flt_fn_%d", g_temp_counter++);
                int lfn = -1, gfn = -1;
                if (g_locals) { lfn = local_add(tfn); bytecode_add_instruction(bc, OP_STORE_LOCAL, lfn); }
                else { gfn = sym_index(tfn); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gfn); }
                bytecode_add_instruction(bc, OP_MAKE_ARRAY, 0);
                char tres[64]; snprintf(tres, sizeof(tres), "__flt_res_%d", g_temp_counter++);
                int lres = -1, gres = -1;
                if (g_locals) { lres = local_add(tres); bytecode_add_instruction(bc, OP_STORE_LOCAL, lres); }
                else { gres = sym_index(tres); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gres); }
                int c0 = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
                char ti[64]; snprintf(ti, sizeof(ti), "__flt_i_%d", g_temp_counter++);
                int li = -1, gi = -1;
                if (g_locals) { li = local_add(ti); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { gi = sym_index(ti); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                int loop_start = bc->instr_count;
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); }
                bytecode_add_instruction(bc, OP_LEN, 0);
                bytecode_add_instruction(bc, OP_LT, 0);
                int jf = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lfn); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gfn); }
                bytecode_add_instruction(bc, OP_SWAP, 0);
                bytecode_add_instruction(bc, OP_CALL, 1);
                /* if truthy then push elem to res */
                int jskip = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                /* Append element to result: res[len(res)] = elem */
                /* Reload element into a temp */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);

                char tvf[64]; snprintf(tvf, sizeof(tvf), "__flt_v_%d", g_temp_counter++);
                int lvf = -1, gvf = -1;
                if (g_locals) { lvf = local_add(tvf); bytecode_add_instruction(bc, OP_STORE_LOCAL, lvf); }
                else { gvf = sym_index(tvf); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gvf); }

                /* Push array */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }

                /* index = len(res) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }
                bytecode_add_instruction(bc, OP_LEN, 0);

                /* value */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lvf); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gvf); }

                /* Append via insert: res.insert(index=len(res), value) */
                bytecode_add_instruction(bc, OP_INSERT, 0);
                /* dApache-2.0ard returned new length */
                bytecode_add_instruction(bc, OP_POP, 0);

                int c1 = bytecode_add_constant(bc, make_int(1));
                /* patch skip over append */
                bytecode_set_operand(bc, jskip, bc->instr_count);
                /* i++ */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_JUMP, loop_start);
                bytecode_set_operand(bc, jf, bc->instr_count);
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lres); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gres); }
                free(name);
                return 1;
            }
            if (strcmp(name, "reduce") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "reduce expects (array, init, function)"); free(name); return 0; }
                char tarr[64]; snprintf(tarr, sizeof(tarr), "__red_arr_%d", g_temp_counter++);
                int larr = -1, garr = -1;
                if (g_locals) { larr = local_add(tarr); bytecode_add_instruction(bc, OP_STORE_LOCAL, larr); }
                else { garr = sym_index(tarr); bytecode_add_instruction(bc, OP_STORE_GLOBAL, garr); }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "reduce expects (array, init, function)"); free(name); return 0; }
                char tacc[64]; snprintf(tacc, sizeof(tacc), "__red_acc_%d", g_temp_counter++);
                int lacc = -1, gacc = -1;
                if (g_locals) { lacc = local_add(tacc); bytecode_add_instruction(bc, OP_STORE_LOCAL, lacc); }
                else { gacc = sym_index(tacc); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gacc); }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "reduce expects (array, init, function)"); free(name); return 0; }
                char tfn[64]; snprintf(tfn, sizeof(tfn), "__red_fn_%d", g_temp_counter++);
                int lfn = -1, gfn = -1;
                if (g_locals) { lfn = local_add(tfn); bytecode_add_instruction(bc, OP_STORE_LOCAL, lfn); }
                else { gfn = sym_index(tfn); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gfn); }
                /* loop */
                int c0 = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
                char ti[64]; snprintf(ti, sizeof(ti), "__red_i_%d", g_temp_counter++);
                int li = -1, gi = -1;
                if (g_locals) { li = local_add(ti); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { gi = sym_index(ti); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                int loop_start = bc->instr_count;
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); }
                bytecode_add_instruction(bc, OP_LEN, 0);
                bytecode_add_instruction(bc, OP_LT, 0);
                int jf = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                /* elem */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr); bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr); bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);

                /* Store elem to a temp so we can build stack as: fn, acc, elem */
                char telem[64]; snprintf(telem, sizeof(telem), "__red_elem_%d", g_temp_counter++);
                int lelem = -1, gelem = -1;
                if (g_locals) { lelem = local_add(telem); bytecode_add_instruction(bc, OP_STORE_LOCAL, lelem); }
                else { gelem = sym_index(telem); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gelem); }

                /* push function */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lfn); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gfn); }

                /* push accumulator (arg1) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lacc); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gacc); }

                /* push element (arg2) */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lelem); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gelem); }

                /* call fn(acc, elem) -> result */
                bytecode_add_instruction(bc, OP_CALL, 2);
                /* store to acc */
                if (g_locals) { bytecode_add_instruction(bc, OP_STORE_LOCAL, lacc); }
                else { bytecode_add_instruction(bc, OP_STORE_GLOBAL, gacc); }
                int c1 = bytecode_add_constant(bc, make_int(1));
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, li); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_LOCAL, li); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi); bytecode_add_instruction(bc, OP_LOAD_CONST, c1); bytecode_add_instruction(bc, OP_ADD, 0); bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi); }
                bytecode_add_instruction(bc, OP_JUMP, loop_start);
                bytecode_set_operand(bc, jf, bc->instr_count);
                /* result = acc on stack */
                if (g_locals) { bytecode_add_instruction(bc, OP_LOAD_LOCAL, lacc); }
                else { bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gacc); }
                free(name);
                return 1;
            }
            if (strcmp(name, "zip") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "zip expects first array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "zip expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "zip expects second array"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after zip args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ZIP, 0);
                free(name);
                return 1;
            }
            /* math */
            if (strcmp(name, "min") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "min expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "min expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_MIN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "max") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "max expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "max expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_MAX, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "fmin") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "fmin expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "fmin expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_FMIN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "fmax") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "fmax expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "fmax expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_FMAX, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "clamp") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "clamp expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "clamp expects 3 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "clamp expects 3 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CLAMP, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "abs") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "abs expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ABS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "floor") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "floor expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_FLOOR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ceil") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "ceil expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_CEIL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "trunc") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "trunc expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TRUNC, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "round") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "round expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ROUND, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sin") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "sin expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SIN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "cos") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "cos expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_COS, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "tan") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "tan expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_TAN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "exp") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "exp expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_EXP, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "log") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "log expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LOG, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "log10") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "log10 expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LOG10, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sqrt") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "sqrt expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SQRT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "gcd") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "gcd expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "gcd expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_GCD, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "lcm") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "lcm expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "lcm expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_LCM, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "isqrt") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "isqrt expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ISQRT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sign") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "sign expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SIGN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pow") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "pow expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "pow expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_POW, 0);
                free(name);
                return 1;
            }

            if (strcmp(name, "random_seed") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "random_seed expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RANDOM_SEED, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "random_int") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "random_int expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "random_int expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RANDOM_INT, 0);
                free(name);
                return 1;
            }

            if (strcmp(name, "random_number") == 0) {
                (*pos)++; /* '(' */
                /* expects exactly 1 arg: length */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "random_number expects 1 arg (length)"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after random_number arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RANDOM_NUMBER, 0);
                free(name);
                return 1;
            }

            /* threading */
            if (strcmp(name, "thread_spawn") == 0) {
                (*pos)++; /* '(' */
                /* thread_spawn(fn [, args]) */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "thread_spawn expects function as first arg"); free(name); return 0; }
                int hasArgs = 0;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] == ',') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "thread_spawn second arg must be array or value"); free(name); return 0; }
                    hasArgs = 1;
                }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after thread_spawn args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_THREAD_SPAWN, hasArgs ? 1 : 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "thread_join") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "thread_join expects 1 arg (thread id)"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_THREAD_JOIN, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "sleep") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "sleep expects 1 arg (milliseconds)"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SLEEP_MS, 0);
                free(name);
                return 1;
            }

            /* bitwise ops (32-bit) */
            if (strcmp(name, "band") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "band expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "band expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_BAND, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "bor") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "bor expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "bor expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_BOR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "bxor") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "bxor expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "bxor expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_BXOR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "bnot") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "bnot expects 1 arg"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "bnot expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_BNOT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "shl") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "shl expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "shl expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SHL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "shr") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "shr expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "shr expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_SHR, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "rol") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "rol expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "rol expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ROTL, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "ror") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "ror expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "ror expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ROTR, 0);
                free(name);
                return 1;
            }

            /* push function value first */
            if (local_idx >= 0) {
                bytecode_add_instruction(bc, OP_LOAD_LOCAL, local_idx);
            } else {
                int gi = sym_index(name);
                bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
            }
            /* Track namespace alias only for the initial receiver; after any call it's no longer an alias value */
            int __ns_ctx = is_ns_alias(name);
            /* parse arguments */
            (*pos)++; /* '(' */
            int argc = 0;
            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] != ')') {
                do {
                    if (!emit_expression(bc, src, len, pos)) {
                        parser_fail(*pos, "Expected expression as function argument");
                        free(name);
                        return 0;
                    }
                    argc++;
                    skip_spaces(src, len, pos);
                } while (*pos < len && src[*pos] == ',' && (++(*pos), skip_spaces(src, len, pos), 1));
            }
            if (!consume_char(src, len, pos, ')')) {
                parser_fail(*pos, "Expected ')' after arguments");
                free(name);
                return 0;
            }
#ifdef FUN_DEBUG
            /* DEBUG: show compiled call */
            printf("compile: CALL %s with %d arg(s)\n", name, argc);
#endif
            bytecode_add_instruction(bc, OP_CALL, argc);
            /* postfix indexing, slice, and dot access/method calls */
            for (;;) {
                skip_spaces(src, len, pos);

                /* index/slice */
                if (*pos < len && src[*pos] == '[') {
                    (*pos)++;
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); free(name); return 0; }
                    skip_spaces(src, len, pos);
                    if (*pos < len && src[*pos] == ':') {
                        (*pos)++;
                        skip_spaces(src, len, pos);
                        size_t svp = *pos;
                        if (!emit_expression(bc, src, len, pos)) {
                            *pos = svp;
                            int ci = bytecode_add_constant(bc, make_int(-1));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                        }
                        if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); free(name); return 0; }
                        bytecode_add_instruction(bc, OP_SLICE, 0);
                        continue;
                    } else {
                        if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); free(name); return 0; }
                        bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                        continue;
                    }
                }

                /* dot property access and method-call sugar: obj.field or obj.method(...) */
                if (*pos < len && src[*pos] == '.') {
                    (*pos)++; /* '.' */
                    skip_spaces(src, len, pos);
                    char *mname = NULL;
                    if (!read_identifier_into(src, len, pos, &mname)) { parser_fail(*pos, "Expected identifier after '.'"); free(name); return 0; }
                    int is_private = (mname && mname[0] == '_');
                    int kci = bytecode_add_constant(bc, make_string(mname));

                    /* Peek for immediate call: obj.method( ... ) */
                    size_t callp = *pos;
                    skip_spaces(src, len, &callp);
                    if (callp < len && src[callp] == '(') {
                        /* If private method on non-'this' receiver in this context -> error */
                        if (is_private) {
                            char msg[160];
                            snprintf(msg, sizeof(msg), "AccessError: private method '%s' is not accessible here", mname);
                            int ci = bytecode_add_constant(bc, make_string(msg));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                            bytecode_add_instruction(bc, OP_PRINT, 0);
                            bytecode_add_instruction(bc, OP_HALT, 0);
                            free(mname);
                            continue;
                        }

                        /* After a function call, the receiver is a value (not a namespace alias);
                           always treat dot-call as a method with implicit 'this'. */
                        int is_ns = 0;

                        /* Method sugar with implicit 'this' */
                        bytecode_add_instruction(bc, OP_DUP, 0);
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                        bytecode_add_instruction(bc, OP_INDEX_GET, 0); /* -> stack: obj, func */
                        bytecode_add_instruction(bc, OP_SWAP, 0);      /* -> stack: func, obj (this) */

                        /* Consume '(' and parse args */
                        *pos = callp + 1;
                        int argc = 0;
                        skip_spaces(src, len, pos);
                        if (*pos < len && src[*pos] != ')') {
                            do {
                                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected expression as method argument"); free(mname); free(name); return 0; }
                                argc++;
                                skip_spaces(src, len, pos);
                            } while (*pos < len && src[*pos] == ',' && (++(*pos), skip_spaces(src, len, pos), 1));
                        }
                        if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after arguments"); free(mname); free(name); return 0; }

                        /* Call */
                        bytecode_add_instruction(bc, OP_CALL, is_ns ? argc : (argc + 1));
                        free(mname);
                        continue;
                    } else {
                        /* Plain property get: obj["field"] */
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                        bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                        free(mname);
                        continue;
                    }
                }

                break;
            }
            free(name);
            return 1;
        } else {
            if (local_idx >= 0) {
                bytecode_add_instruction(bc, OP_LOAD_LOCAL, local_idx);
            } else {
                int gi = sym_index(name);
                bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
            }
            /* track namespace alias only for the initial receiver; after any call it's no longer an alias value */
            int __ns_ctx = is_ns_alias(name);
            /* postfix indexing, slice, and dot access/method calls */
            for (;;) {
                skip_spaces(src, len, pos);

                /* index/slice */
                if (*pos < len && src[*pos] == '[') {
                    (*pos)++;
                    if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected start expression"); free(name); return 0; }
                    skip_spaces(src, len, pos);
                    if (*pos < len && src[*pos] == ':') {
                        (*pos)++;
                        skip_spaces(src, len, pos);
                        size_t svp = *pos;
                        if (!emit_expression(bc, src, len, pos)) {
                            *pos = svp;
                            int ci = bytecode_add_constant(bc, make_int(-1));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                        }
                        if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after slice"); free(name); return 0; }
                        bytecode_add_instruction(bc, OP_SLICE, 0);
                        continue;
                    } else {
                        if (!consume_char(src, len, pos, ']')) { parser_fail(*pos, "Expected ']' after index"); free(name); return 0; }
                        bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                        continue;
                    }
                }

                /* dot property access and method-call sugar */
                if (*pos < len && src[*pos] == '.') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    char *mname = NULL;
                    if (!read_identifier_into(src, len, pos, &mname)) { parser_fail(*pos, "Expected identifier after '.'"); free(name); return 0; }
                    int is_private = (mname && mname[0] == '_');
                    int kci = bytecode_add_constant(bc, make_string(mname));

                    /* Peek for call */
                    size_t callp = *pos;
                    skip_spaces(src, len, &callp);
                    if (callp < len && src[callp] == '(') {
                        /* If private and receiver is not 'this' -> error */
                        if (is_private && !(strcmp(name, "this") == 0)) {
                            char msg[160];
                            snprintf(msg, sizeof(msg), "AccessError: private method '%s' is not accessible", mname);
                            int ci = bytecode_add_constant(bc, make_string(msg));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                            bytecode_add_instruction(bc, OP_PRINT, 0);
                            bytecode_add_instruction(bc, OP_HALT, 0);
                            free(mname);
                            continue;
                        }

                        /* Use initial alias context for only the first dot-call; reset after call */
                        int is_ns = __ns_ctx;

                        if (!is_ns) {
                            /* Method sugar with implicit 'this' */
                            bytecode_add_instruction(bc, OP_DUP, 0);
                            bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                            bytecode_add_instruction(bc, OP_INDEX_GET, 0); /* -> obj, func */
                            bytecode_add_instruction(bc, OP_SWAP, 0);      /* -> func, obj */
                        } else {
                            /* Plain property function call */
                            bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                            bytecode_add_instruction(bc, OP_INDEX_GET, 0); /* -> func */
                        }

                        *pos = callp + 1;
                        int argc = 0;
                        skip_spaces(src, len, pos);
                        if (*pos < len && src[*pos] != ')') {
                            do {
                                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "Expected expression as method argument"); free(mname); free(name); return 0; }
                                argc++;
                                skip_spaces(src, len, pos);
                            } while (*pos < len && src[*pos] == ',' && (++(*pos), skip_spaces(src, len, pos), 1));
                        }
                        if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after arguments"); free(mname); free(name); return 0; }

                        bytecode_add_instruction(bc, OP_CALL, is_ns ? argc : (argc + 1));
                        /* After any call, the receiver is now a value, not a namespace alias */
                        __ns_ctx = 0;

                        free(mname);
                        continue;
                    } else {
                        /* plain property get (allowed even for private name) */
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                        bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                        free(mname);
                        continue;
                    }
                }

                break;
            }
            free(name);
            return 1;
        }
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

        /* Stop expression at start of inline comment */
        if (*pos + 1 < len && src[*pos] == '/' && src[*pos + 1] == '/') {
            break;
        }
        /* Skip block comments inside expressions */
        if (*pos + 1 < len && src[*pos] == '/' && src[*pos + 1] == '*') {
            size_t p = *pos + 2;
            while (p + 1 < len && !(src[p] == '*' && src[p + 1] == '/')) {
                p++;
            }
            if (p + 1 < len) p += 2; /* consume closing marker */
            *pos = p;
            continue;
        }

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

/* logical AND with short-circuit: equality ( '&&' equality )* */
static int emit_and_expr(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    int jf_idxs[64];
    int jf_count = 0;
    int has_and = 0;

    /* first operand */
    if (!emit_equality(bc, src, len, pos)) return 0;

    for (;;) {
        skip_spaces(src, len, pos);
        if (!(*pos + 1 < len && src[*pos] == '&' && src[*pos + 1] == '&')) break;
        *pos += 2;
        has_and = 1;

        /* if current value is false -> jump to false label (patched later) */
        if (jf_count < (int)(sizeof(jf_idxs) / sizeof(jf_idxs[0]))) {
            jf_idxs[jf_count++] = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
        } else {
            parser_fail(*pos, "Too many operands in '&&' chain");
            return 0;
        }

        /* evaluate next operand */
        if (!emit_equality(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after '&&'");
            return 0;
        }
    }

    if (has_and) {
        /* final: if last operand is false -> jump false */
        if (jf_count < (int)(sizeof(jf_idxs) / sizeof(jf_idxs[0]))) {
            jf_idxs[jf_count++] = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
        } else {
            parser_fail(*pos, "Too many operands in '&&' chain");
            return 0;
        }

        /* all were truthy -> result true */
        int c1 = bytecode_add_constant(bc, make_bool(1));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
        int j_end = bytecode_add_instruction(bc, OP_JUMP, 0);

        /* false label: patch all false jumps here, result false */
        int l_false = bc->instr_count;
        for (int i = 0; i < jf_count; ++i) {
            bytecode_set_operand(bc, jf_idxs[i], l_false);
        }
        int c0 = bytecode_add_constant(bc, make_bool(0));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c0);

        /* end */
        int l_end = bc->instr_count;
        bytecode_set_operand(bc, j_end, l_end);
    }

    return 1;
}

/* logical OR with short-circuit: and_expr ( '||' and_expr )* */
static int emit_or_expr(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    int true_jumps[64];
    int tj_count = 0;
    int has_or = 0;

    /* first operand */
    if (!emit_and_expr(bc, src, len, pos)) return 0;

    for (;;) {
        skip_spaces(src, len, pos);
        if (!(*pos + 1 < len && src[*pos] == '|' && src[*pos + 1] == '|')) break;
        *pos += 2;
        has_or = 1;

        /* if current value is false -> proceed to next; else -> result true */
        int jf_proceed = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

        /* true path: push true and jump to end */
        int c1 = bytecode_add_constant(bc, make_bool(1));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
        if (tj_count < (int)(sizeof(true_jumps) / sizeof(true_jumps[0]))) {
            true_jumps[tj_count++] = bytecode_add_instruction(bc, OP_JUMP, 0);
        } else {
            parser_fail(*pos, "Too many operands in '||' chain");
            return 0;
        }

        /* patch to start of next operand */
        bytecode_set_operand(bc, jf_proceed, bc->instr_count);

        /* evaluate next operand */
        if (!emit_and_expr(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after '||'");
            return 0;
        }
    }

    if (has_or) {
        /* After evaluating the last operand: test it and produce 1/0 */
        int jf_last = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

        int c1 = bytecode_add_constant(bc, make_int(1));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
        int j_end_single = bytecode_add_instruction(bc, OP_JUMP, 0);

        int l_false = bc->instr_count;
        bytecode_set_operand(bc, jf_last, l_false);
        int c0 = bytecode_add_constant(bc, make_int(0));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c0);

        int l_end = bc->instr_count;
        bytecode_set_operand(bc, j_end_single, l_end);
        for (int i = 0; i < tj_count; ++i) {
            bytecode_set_operand(bc, true_jumps[i], l_end);
        }
    }

    return 1;
}

/* conditional operator (ternary) with right associativity:
   Parses: logical_or ('?' conditional ':' conditional)? */
static int emit_conditional(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    /* parse condition (logical OR precedence or higher) */
    if (!emit_or_expr(bc, src, len, pos)) return 0;

    for (;;) {
        skip_spaces(src, len, pos);
        if (!(*pos < len && src[*pos] == '?')) break;
        (*pos)++; /* consume '?' */

        /* If condition is false -> jump to false arm */
        int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

        /* true arm (right-assoc: allow nested ternaries) */
        skip_spaces(src, len, pos);
        if (!emit_conditional(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after '?'");
            return 0;
        }

        /* After true arm, unconditionally skip false arm */
        int jmp_end = bytecode_add_instruction(bc, OP_JUMP, 0);

        /* false arm label */
        bytecode_set_operand(bc, jmp_false, bc->instr_count);

        /* require ':' */
        skip_spaces(src, len, pos);
        if (!(*pos < len && src[*pos] == ':')) {
            parser_fail(*pos, "Expected ':' in conditional expression");
            return 0;
        }
        (*pos)++; /* consume ':' */

        /* false arm (right-assoc) */
        skip_spaces(src, len, pos);
        if (!emit_conditional(bc, src, len, pos)) {
            parser_fail(*pos, "Expected expression after ':'");
            return 0;
        }

        /* end label */
        bytecode_set_operand(bc, jmp_end, bc->instr_count);
        /* loop to allow chaining like a ? b : c ? d : e (right-assoc) */
    }
    return 1;
}

/* top-level expression */
static int emit_expression(Bytecode *bc, const char *src, size_t len, size_t *pos) {
    return emit_conditional(bc, src, len, pos);
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
    /* Strict mode: only allow trailing spaces and comments until end-of-line. */
    size_t p = *pos;

    for (;;) {
        /* skip spaces */
        while (p < len && src[p] == ' ') p++;

        if (p >= len) { *pos = p; return; }
        /* Treat CR, LF, and CRLF as end-of-line */
        if (src[p] == '\r') {
            p++;
            if (p < len && src[p] == '\n') p++;
            *pos = p;
            return;
        }
        if (src[p] == '\n') {
            *pos = p + 1;
            return;
        }

        /* line or block comments allowed */
        if (p + 1 < len && src[p] == '/' && src[p + 1] == '/') {
            /* consume rest of line up to CR or LF */
            p += 2;
            while (p < len && src[p] != '\n' && src[p] != '\r') p++;
            if (p < len && src[p] == '\r') {
                p++;
                if (p < len && src[p] == '\n') p++;
            } else if (p < len && src[p] == '\n') {
                p++;
            }
            *pos = p;
            return;
        }

        if (p + 1 < len && src[p] == '/' && src[p + 1] == '*') {
            /* consume block comment, then loop again for spaces till EOL */
            p += 2;
            while (p + 1 < len && !(src[p] == '*' && src[p + 1] == '/')) {
                p++;
            }
            if (p + 1 < len) {
                p += 2; /* consume closing */ 
                continue;
            } else {
                parser_fail(p, "Unterminated block comment at end of file");
                *pos = p;
                return;
            }
        }

        /* Any other character here is unexpected trailing garbage */
        parser_fail(p, "Unexpected trailing characters at end of line");
        *pos = p;
        return;
    }
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

        /* empty line: handle CR, LF, and CRLF */
        if (src[p] == '\r') {
            p++;
            if (p < len && src[p] == '\n') p++;
            *pos = p;
            continue;
        }
        if (src[p] == '\n') { p++; *pos = p; continue; }

        /* // comment-only line */
        if (p + 1 < len && src[p] == '/' && src[p + 1] == '/') {
            /* skip entire line up to CR/LF */
            p += 2;
            while (p < len && src[p] != '\n' && src[p] != '\r') p++;
            if (p < len && src[p] == '\r') { p++; if (p < len && src[p] == '\n') p++; }
            else if (p < len && src[p] == '\n') { p++; }
            *pos = p;
            continue;
        }

        // block comment starting at line (treat as comment-only line)
        if (p + 1 < len && src[p] == '/' && src[p + 1] == '*') {
            p += 2;
            /* advance until we find closing block comment marker */
            while (p + 1 < len && !(src[p] == '*' && src[p + 1] == '/')) {
                p++;
            }
            if (p + 1 < len) p += 2; /* consume closing block comment marker */
            /* consume to end of current line (if any leftover) */
            while (p < len && src[p] != '\n' && src[p] != '\r') p++;
            if (p < len && src[p] == '\r') { p++; if (p < len && src[p] == '\n') p++; }
            else if (p < len && src[p] == '\n') { p++; }
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

        /* alias: accept 'sint*' as synonyms for 'int*' */
        if (strcmp(name, "sint8") == 0)  { free(name); name = strdup("int8");  }
        else if (strcmp(name, "sint16") == 0) { free(name); name = strdup("int16"); }
        else if (strcmp(name, "sint32") == 0) { free(name); name = strdup("int32"); }
        else if (strcmp(name, "sint64") == 0) { free(name); name = strdup("int64"); }

        /* return statement */
        if (strcmp(name, "return") == 0) {
            free(name);
            skip_spaces(src, len, &local_pos);
            /* optional expression */
            size_t save_pos = local_pos;
            if (emit_expression(bc, src, len, &local_pos)) {
                /* expression result already on stack */
            } else {
                /* no expression: return nil */
                local_pos = save_pos;
                int ci = bytecode_add_constant(bc, make_nil());
                bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
            }
            bytecode_add_instruction(bc, OP_RETURN, 0);
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* exit statement: exit [expr]? */
        if (strcmp(name, "exit") == 0) {
            free(name);
            skip_spaces(src, len, &local_pos);
            size_t save_pos = local_pos;
            if (emit_expression(bc, src, len, &local_pos)) {
                /* expression result already on stack */
            } else {
                /* default exit code 0 */
                local_pos = save_pos;
                int ci = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
            }
            bytecode_add_instruction(bc, OP_EXIT, 0);
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* break / continue */
        if (strcmp(name, "break") == 0) {
            free(name);
            if (!g_loop_ctx) {
                parser_fail(local_pos, "break used outside of loop");
                return;
            }
            int j = bytecode_add_instruction(bc, OP_JUMP, 0);
            if (g_loop_ctx->break_count < (int)(sizeof(g_loop_ctx->break_jumps) / sizeof(g_loop_ctx->break_jumps[0]))) {
                g_loop_ctx->break_jumps[g_loop_ctx->break_count++] = j;
            } else {
                parser_fail(local_pos, "Too many 'break' in one loop");
                return;
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }
        if (strcmp(name, "continue") == 0) {
            free(name);
            if (!g_loop_ctx) {
                parser_fail(local_pos, "continue used outside of loop");
                return;
            }
            int j = bytecode_add_instruction(bc, OP_JUMP, 0);
            if (g_loop_ctx->cont_count < (int)(sizeof(g_loop_ctx->continue_jumps) / sizeof(g_loop_ctx->continue_jumps[0]))) {
                g_loop_ctx->continue_jumps[g_loop_ctx->cont_count++] = j;
            } else {
                parser_fail(local_pos, "Too many 'continue' in one loop");
                return;
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* typed declarations:
           number|string|boolean|nil|Class|byte|uint8|uint16|uint32|uint64|int8|int16|int32|int64 <ident> (= expr)?
           Note: 'number' maps to signed 64-bit here. 'byte' is an alias of unsigned 8-bit. 'Class' restricts to class instances.
         */
        if (strcmp(name, "number") == 0 || strcmp(name, "string") == 0 || strcmp(name, "boolean") == 0 || strcmp(name, "nil") == 0
            || strcmp(name, "class") == 0 || strcmp(name, "float") == 0
            || strcmp(name, "array") == 0
            || strcmp(name, "byte") == 0
            || strcmp(name, "uint8") == 0 || strcmp(name, "uint16") == 0 || strcmp(name, "uint32") == 0 || strcmp(name, "uint64") == 0
            || strcmp(name, "int8") == 0  || strcmp(name, "int16") == 0  || strcmp(name, "int32") == 0  || strcmp(name, "int64") == 0) {
            int is_number  = (strcmp(name, "number") == 0);
            int is_string  = (strcmp(name, "string") == 0);
            int is_boolean = (strcmp(name, "boolean") == 0);
            int is_nil     = (strcmp(name, "nil") == 0);
            int is_class = (strcmp(name, "class") == 0);
            int is_float = (strcmp(name, "float") == 0);
            int is_array = (strcmp(name, "array") == 0);
            int is_byte    = (strcmp(name, "byte")   == 0);
            int is_u8      = (strcmp(name, "uint8")  == 0) || is_byte;
            int is_u16     = (strcmp(name, "uint16") == 0);
            int is_u32     = (strcmp(name, "uint32") == 0);
            int is_u64     = (strcmp(name, "uint64") == 0);
            int is_s8      = (strcmp(name, "int8")   == 0);
            int is_s16     = (strcmp(name, "int16")  == 0);
            int is_s32     = (strcmp(name, "int32")  == 0);
            int is_s64     = (strcmp(name, "int64")  == 0) || is_number; /* number maps to int64 (signed) */
            int decl_bits  = is_u8 ? 8 : is_u16 ? 16 : is_u32 ? 32 : is_u64 ? 64
                           : is_s8 ? 8 : is_s16 ? 16 : is_s32 ? 32 : is_s64 ? 64 : 0;
            int decl_signed = (is_s8 || is_s16 || is_s32 || is_s64) ? 1 : 0;
            /* store decl bits with sign encoded: negative means signed (number is signed 64-bit) */
            if (decl_signed) decl_bits = -decl_bits;

            /* declared type metadata: integers use decl_bits; string/boolean/nil/Class/float/array use special markers */
            int decl_meta = decl_bits;
            if (is_string) {
                decl_meta = TYPE_META_STRING;
            } else if (is_boolean) {
                decl_meta = TYPE_META_BOOLEAN;
            } else if (is_nil) {
                decl_meta = TYPE_META_NIL;
            } else if (is_class) {
                decl_meta = TYPE_META_CLASS;
            } else if (is_float) {
                decl_meta = TYPE_META_FLOAT;
            } else if (is_array) {
                decl_meta = TYPE_META_ARRAY;
            }

            free(name);

            /* read variable name */
            char *varname = NULL;
            skip_spaces(src, len, &local_pos);
            if (!read_identifier_into(src, len, &local_pos, &varname)) {
                parser_fail(local_pos, "Expected identifier after type declaration");
                return;
            }

            /* decide local vs global */
            int lidx = -1;
            int gi = -1;
            if (g_locals) {
                int existing = local_find(varname);
                if (existing >= 0) lidx = existing;
                else lidx = local_add(varname);
                if (lidx >= 0) {
                    g_locals->types[lidx] = decl_meta; /* encoding: ±bits for integers; TYPE_META_* for non-integer enforced types; 0 = dynamic */
                }
            } else {
                gi = sym_index(varname);
                if (gi >= 0) {
                    G.types[gi] = decl_meta;
                }
            }
            free(varname);

            skip_spaces(src, len, &local_pos);
            if (local_pos < len && src[local_pos] == '=') {
                local_pos++; /* '=' */
                if (!emit_expression(bc, src, len, &local_pos)) {
                    parser_fail(local_pos, "Expected initializer expression after '='");
                    return;
                }

                /* Enforce declared type on initializer */
                if (decl_meta == TYPE_META_STRING) {
                    /* expect String */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciExp = bytecode_add_constant(bc, make_string("String"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciExp);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    /* error block */
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected String"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (decl_meta == TYPE_META_CLASS) {
                    /* expect Class instance: Map with "__class" key */
                    /* Check typeof(v) == "Map" */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    {
                        int ciMap = bytecode_add_constant(bc, make_string("Map"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMap);
                    }
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_err1 = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* Check v has "__class" */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    {
                        int kci = bytecode_add_constant(bc, make_string("__class"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                    }
                    bytecode_add_instruction(bc, OP_HAS_KEY, 0);
                    int j_err2 = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* Success path jumps over error block */
                    int j_ok = bytecode_add_instruction(bc, OP_JUMP, 0);
                    /* Error block */
                    int err_lbl = bc->instr_count;
                    bytecode_set_operand(bc, j_err1, err_lbl);
                    bytecode_set_operand(bc, j_err2, err_lbl);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Class"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    /* Continue after checks */
                    bytecode_set_operand(bc, j_ok, bc->instr_count);
                } else if (decl_meta == TYPE_META_FLOAT) {
                    /* expect Float */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciF = bytecode_add_constant(bc, make_string("Float"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciF);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Float"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (decl_meta == TYPE_META_ARRAY) {
                    /* expect Array */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciArr = bytecode_add_constant(bc, make_string("Array"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciArr);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Array"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (decl_meta == TYPE_META_BOOLEAN) {
                    /* accept Boolean literal or Number; if Number, clamp to 0/1 */
                    /* check if value is Boolean */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciBool = bytecode_add_constant(bc, make_string("Boolean"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciBool);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_not_bool = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* it is Boolean -> OK, skip number check */
                    int j_done = bytecode_add_instruction(bc, OP_JUMP, 0);
                    /* not Boolean: check Number */
                    bytecode_set_operand(bc, j_not_bool, bc->instr_count);
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciNum = bytecode_add_constant(bc, make_string("Number"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciNum);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Boolean or Number for boolean"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                    /* if Number, clamp to 0/1 */
                    bytecode_add_instruction(bc, OP_UCLAMP, 1);
                    /* common continuation */
                    bytecode_set_operand(bc, j_done, bc->instr_count);
                } else if (decl_meta == TYPE_META_NIL) {
                    /* expect Nil */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciNil = bytecode_add_constant(bc, make_string("Nil"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciNil);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Nil"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else {
                    /* integer widths: expect Number then range-check */
                    int abs_bits = decl_bits < 0 ? -decl_bits : decl_bits;
                    if (abs_bits > 0) {
                        /* typeof == Number */
                        bytecode_add_instruction(bc, OP_DUP, 0);
                        bytecode_add_instruction(bc, OP_TYPEOF, 0);
                        int ciNum = bytecode_add_constant(bc, make_string("Number"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciNum);
                        bytecode_add_instruction(bc, OP_EQ, 0);
                        int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                        int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                        bytecode_set_operand(bc, j_to_error, bc->instr_count);
                        {
                            int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Number"));
                            bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                            bytecode_add_instruction(bc, OP_PRINT, 0);
                            bytecode_add_instruction(bc, OP_HALT, 0);
                        }
                        bytecode_set_operand(bc, j_skip_err, bc->instr_count);

                        if (abs_bits > 0) {
                            bytecode_add_instruction(bc, (decl_bits < 0) ? OP_SCLAMP : OP_UCLAMP, abs_bits);
                        }
                    }
                }

                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }
            } else {
                /* default initialize if no '=' given */
                int ci = -1;
                if (is_string) {
                    ci = bytecode_add_constant(bc, make_string(""));
                } else if (is_nil) {
                    ci = bytecode_add_constant(bc, make_nil());
                } else if (is_class) {
                    /* Class-typed variable defaults to Nil until assigned an instance */
                    ci = bytecode_add_constant(bc, make_nil());
                } else if (is_boolean) {
                    /* booleans default to false */
                    ci = bytecode_add_constant(bc, make_bool(0));
                } else if (is_number || (decl_bits != 0)) {
                    /* integers default to 0 */
                    ci = bytecode_add_constant(bc, make_int(0));
                }
                if (ci >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
                    int abs_bits2 = decl_bits < 0 ? -decl_bits : decl_bits;
                    if (abs_bits2 > 0) {
                        bytecode_add_instruction(bc, (decl_bits < 0) ? OP_SCLAMP : OP_UCLAMP, abs_bits2);
                    }
                    if (lidx >= 0) {
                        bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                    } else {
                        bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                    }
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

        if (strcmp(name, "echo") == 0) {
            free(name);
            skip_spaces(src, len, &local_pos);
            (void)consume_char(src, len, &local_pos, '(');
            if (emit_expression(bc, src, len, &local_pos)) {
                (void)consume_char(src, len, &local_pos, ')');
                bytecode_add_instruction(bc, OP_ECHO, 0);
            } else {
                (void)consume_char(src, len, &local_pos, ')');
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* assignment or simple call */
        int lidx = local_find(name);
        int gi = (lidx < 0) ? sym_index(name) : -1;
        skip_spaces(src, len, &local_pos);

        /* object field assignment: name.field = expr (only if '=' follows) */
        if (local_pos < len && src[local_pos] == '.') {
            size_t stmt_start = *pos;       /* for expression fallback */
            size_t look = local_pos + 1;    /* point after '.' */
            skip_spaces(src, len, &look);
            char *fname = NULL;
            if (!read_identifier_into(src, len, &look, &fname)) {
                parser_fail(look, "Expected field name after '.'");
                free(name);
                return;
            }
            skip_spaces(src, len, &look);
            if (look >= len || src[look] != '=') {
                /* Not an assignment: treat as expression statement (e.g., obj.method(...)) */
                free(fname);
                free(name);
                size_t expr_pos = stmt_start;
                if (emit_expression(bc, src, len, &expr_pos)) {
                    bytecode_add_instruction(bc, OP_POP, 0);
                }
                *pos = expr_pos;
                skip_to_eol(src, len, pos);
                return;
            }

            /* Confirmed assignment: emit container, key, value, then INDEX_SET */
            /* Load container variable */
            if (lidx >= 0) {
                bytecode_add_instruction(bc, OP_LOAD_LOCAL, lidx);
            } else {
                bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
            }
            /* Push key */
            int kci = bytecode_add_constant(bc, make_string(fname));
            free(fname);
            bytecode_add_instruction(bc, OP_LOAD_CONST, kci);

            /* Advance local_pos to after '=' and parse value expression */
            local_pos = look + 1; /* skip '=' */
            if (!emit_expression(bc, src, len, &local_pos)) {
                parser_fail(local_pos, "Expected expression after '='");
                free(name);
                return;
            }
            /* perform set: pops value, key, container (in that order) */
            bytecode_add_instruction(bc, OP_INDEX_SET, 0);

            free(name);
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        /* array element assignment: name[expr] = expr and nested: name[expr1][expr2] = expr */
        if (local_pos < len && src[local_pos] == '[') {
            /* load array/map variable */
            if (lidx >= 0) {
                bytecode_add_instruction(bc, OP_LOAD_LOCAL, lidx);
            } else {
                bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
            }
            local_pos++; /* '[' */
            if (!emit_expression(bc, src, len, &local_pos)) {
                parser_fail(local_pos, "Expected index expression after '['");
                free(name);
                return;
            }
            if (!consume_char(src, len, &local_pos, ']')) {
                parser_fail(local_pos, "Expected ']' after index");
                free(name);
                return;
            }
            skip_spaces(src, len, &local_pos);

            /* Nested index: name[expr1][expr2] = value */
            if (local_pos < len && src[local_pos] == '[') {
                /* Reduce base: stack currently has container, index1 -> get inner container */
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);

                local_pos++; /* second '[' */
                if (!emit_expression(bc, src, len, &local_pos)) {
                    parser_fail(local_pos, "Expected nested index expression after '['");
                    free(name);
                    return;
                }
                if (!consume_char(src, len, &local_pos, ']')) {
                    parser_fail(local_pos, "Expected ']' after nested index");
                    free(name);
                    return;
                }
                skip_spaces(src, len, &local_pos);
                if (local_pos >= len || src[local_pos] != '=') {
                    parser_fail(local_pos, "Expected '=' after nested array index");
                    free(name);
                    return;
                }
                local_pos++; /* '=' */
                if (!emit_expression(bc, src, len, &local_pos)) {
                    parser_fail(local_pos, "Expected expression after '='");
                    free(name);
                    return;
                }
                /* perform set into inner container */
                bytecode_add_instruction(bc, OP_INDEX_SET, 0);
                free(name);
                *pos = local_pos;
                skip_to_eol(src, len, pos);
                return;
            }

            /* Single-level: name[expr] = value */
            if (local_pos >= len || src[local_pos] != '=') {
                parser_fail(local_pos, "Expected '=' after array index");
                free(name);
                return;
            }
            local_pos++; /* '=' */
            if (!emit_expression(bc, src, len, &local_pos)) {
                parser_fail(local_pos, "Expected expression after '='");
                free(name);
                return;
            }
            /* perform set */
            bytecode_add_instruction(bc, OP_INDEX_SET, 0);
            free(name);
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        }

        free(name);
        if (local_pos < len && src[local_pos] == '=') {
            local_pos++; /* '=' */
            if (emit_expression(bc, src, len, &local_pos)) {
                /* enforce declared type if present (0 = dynamic) */
                int meta = 0;
                if (lidx >= 0 && g_locals) {
                    meta = g_locals->types[lidx];
                } else if (gi >= 0) {
                    meta = G.types[gi];
                }

                if (meta == TYPE_META_STRING) {
                    /* expect String */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciStr = bytecode_add_constant(bc, make_string("String"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciStr);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected String"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (meta == TYPE_META_CLASS) {
                    /* expect Class instance: Map with "__class" key */
                    /* Check typeof(v) == "Map" */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    {
                        int ciMap = bytecode_add_constant(bc, make_string("Map"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMap);
                    }
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_err1 = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* Check v has "__class" */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    {
                        int kci = bytecode_add_constant(bc, make_string("__class"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                    }
                    bytecode_add_instruction(bc, OP_HAS_KEY, 0);
                    int j_err2 = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    /* Success path jumps over error block */
                    int j_ok = bytecode_add_instruction(bc, OP_JUMP, 0);
                    /* Error block */
                    int err_lbl = bc->instr_count;
                    bytecode_set_operand(bc, j_err1, err_lbl);
                    bytecode_set_operand(bc, j_err2, err_lbl);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Class"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    /* Continue after checks */
                    bytecode_set_operand(bc, j_ok, bc->instr_count);
                } else if (meta == TYPE_META_FLOAT) {
                    /* expect Float */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciF = bytecode_add_constant(bc, make_string("Float"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciF);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Float"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (meta == TYPE_META_ARRAY) {
                    /* expect Array */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciArr = bytecode_add_constant(bc, make_string("Array"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciArr);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Array"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (meta == TYPE_META_BOOLEAN) {
                    /* expect Number then clamp to 1 bit (unsigned) */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciNum = bytecode_add_constant(bc, make_string("Number"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciNum);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Number for boolean"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                    bytecode_add_instruction(bc, OP_UCLAMP, 1);
                } else if (meta == TYPE_META_NIL) {
                    /* expect Nil */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciNil = bytecode_add_constant(bc, make_string("Nil"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciNil);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Nil"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);
                } else if (meta != 0) {
                    /* integer widths: expect Number then range-check to declared width */
                    int abs_bits = meta < 0 ? -meta : meta;
                    /* typeof == Number */
                    bytecode_add_instruction(bc, OP_DUP, 0);
                    bytecode_add_instruction(bc, OP_TYPEOF, 0);
                    int ciNum = bytecode_add_constant(bc, make_string("Number"));
                    bytecode_add_instruction(bc, OP_LOAD_CONST, ciNum);
                    bytecode_add_instruction(bc, OP_EQ, 0);
                    int j_to_error = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);
                    int j_skip_err = bytecode_add_instruction(bc, OP_JUMP, 0);
                    bytecode_set_operand(bc, j_to_error, bc->instr_count);
                    {
                        int ciMsg = bytecode_add_constant(bc, make_string("TypeError: expected Number"));
                        bytecode_add_instruction(bc, OP_LOAD_CONST, ciMsg);
                        bytecode_add_instruction(bc, OP_PRINT, 0);
                        bytecode_add_instruction(bc, OP_HALT, 0);
                    }
                    bytecode_set_operand(bc, j_skip_err, bc->instr_count);

                        if (abs_bits > 0) {
                            bytecode_add_instruction(bc, (meta < 0) ? OP_SCLAMP : OP_UCLAMP, abs_bits);
                        }
                    }
                /* dynamic (meta==0): no enforcement */

                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        } else if (local_pos < len && src[local_pos] == '(') {
            /* call as a statement: compile full call expression and drop result */
            /* rewind to statement start and emit as expression */
            local_pos = *pos;
            if (emit_expression(bc, src, len, &local_pos)) {
                bytecode_add_instruction(bc, OP_POP, 0); /* dApache-2.0ard return value */
            }
            *pos = local_pos;
            skip_to_eol(src, len, pos);
            return;
        } else {
            /* invalid statement: identifier not followed by assignment or call */
            parser_fail(local_pos, "Expected assignment '=' or call '(...)' after identifier");
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

    /* echo(expr): like print but does not add a newline (immediate output) */
    if (starts_with(src, len, *pos, "echo")) {
        *pos += 4;
        skip_spaces(src, len, pos);
        (void)consume_char(src, len, pos, '(');
        if (emit_expression(bc, src, len, pos)) {
            (void)consume_char(src, len, pos, ')');
            bytecode_add_instruction(bc, OP_ECHO, 0);
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
        /* Insert a line marker for better runtime error reporting */
        {
            int stmt_line = 1, stmt_col = 1;
            calc_line_col(src, len, line_start, &stmt_line, &stmt_col);
            bytecode_add_instruction(bc, OP_LINE, stmt_line);
        }

        /* class definition -> factory function */
        if (starts_with(src, len, *pos, "class")) {
            *pos += 5;
            skip_spaces(src, len, pos);
            /* class name */
            char *cname = NULL;
            if (!read_identifier_into(src, len, pos, &cname)) {
                parser_fail(*pos, "Expected class name after 'class'");
                return;
            }
            int cgi = sym_index(cname);

            /* optional extends Parent */
            char *parent_name = NULL;

            /* Optional typed parameter list: class Name(type ident, ...) */
            char *param_names[64]; int param_kind[64]; int pcount = 0;
            memset(param_names, 0, sizeof(param_names));
            memset(param_kind, 0, sizeof(param_kind));

            /* kind: 1=Number (numeric types incl. boolean), 2=String, 3=Nil */
            /* helper macro instead of nested function (C99 compliant) */
            #define MAP_TYPE_KIND(t) ( \
                ((t) && strcmp((t), "string")==0) ? 2 : \
                ((t) && strcmp((t), "nil")==0) ? 3 : \
                ((t) && (strcmp((t), "boolean")==0 || strcmp((t), "number")==0 || strcmp((t), "byte")==0 || strncmp((t), "uint", 4)==0 || strncmp((t), "sint", 4)==0 || strncmp((t), "int", 3)==0)) ? 1 : \
                0 )

            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] == '(') {
                (*pos)++;
                skip_spaces(src, len, pos);
                if (*pos < len && src[*pos] != ')') {
                    for (;;) {
                        /* read type token */
                        char *tname = NULL;
                        if (!read_identifier_into(src, len, pos, &tname)) {
                            parser_fail(*pos, "Expected type in class parameter list");
                            free(cname);
                            return;
                        }
                        /* read param name */
                        skip_spaces(src, len, pos);
                        char *pname = NULL;
                        if (!read_identifier_into(src, len, pos, &pname)) {
                            parser_fail(*pos, "Expected parameter name after type");
                            free(tname);
                            free(cname);
                            return;
                        }
                        if (pcount >= (int)(sizeof(param_names)/sizeof(param_names[0]))) {
                            parser_fail(*pos, "Too many class parameters");
                            free(tname); free(pname); free(cname);
                            return;
                        }
                        param_names[pcount] = pname;
                        param_kind[pcount] = MAP_TYPE_KIND(tname);
                        free(tname);
                        pcount++;

                        skip_spaces(src, len, pos);
                        if (*pos < len && src[*pos] == ',') {
                            (*pos)++;
                            skip_spaces(src, len, pos);
                            continue;
                        }
                        break;
                    }
                }
                if (!consume_char(src, len, pos, ')')) {
                    parser_fail(*pos, "Expected ')' after class parameter list");
                    for (int i = 0; i < pcount; ++i) free(param_names[i]);
                    free(cname);
                    return;
                }
            }

            /* optional 'extends Parent' after parameter list */
            skip_spaces(src, len, pos);
            if (starts_with(src, len, *pos, "extends")) {
                *pos += 7; /* consume 'extends' */
                skip_spaces(src, len, pos);
                if (!read_identifier_into(src, len, pos, &parent_name)) {
                    parser_fail(*pos, "Expected parent class name after 'extends'");
                    for (int i = 0; i < pcount; ++i) free(param_names[i]);
                    free(cname);
                    return;
                }
            }

            /* end of class header line */
            skip_to_eol(src, len, pos);

            /* Build factory function: Name(...) -> instance map with fields and methods */
            Bytecode *ctor_bc = bytecode_new();
                        /* set debug metadata for class factory */
                        if (ctor_bc) {
                            if (ctor_bc->name) free((void*)ctor_bc->name);
                            ctor_bc->name = strdup(cname);
                            if (ctor_bc->source_file) free((void*)ctor_bc->source_file);
                            if (g_current_source_path) ctor_bc->source_file = strdup(g_current_source_path);
                        }
            /* local env for the factory to allow temp locals */
            LocalEnv ctor_env;
            memset(&ctor_env, 0, sizeof(ctor_env));
            LocalEnv *prev_env = g_locals;
            g_locals = &ctor_env;

            /* track if _construct is defined in this class */
            int ctor_present = 0;

            /* Register parameter locals first so args land at 0..pcount-1 */
            for (int i = 0; i < pcount; ++i) {
                local_add(param_names[i]);
            }

            /* Guard local to detect extra argument at index == pcount */
            int l_extra = local_add("__extra");

            /* Runtime checks: missing args and type checks */
            for (int i = 0; i < pcount; ++i) {
                /* missing arg: local i must not be Nil */
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, i);
                bytecode_add_instruction(ctor_bc, OP_TYPEOF, 0);
                int ci_nil = bytecode_add_constant(ctor_bc, make_string("Nil"));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_nil);
                bytecode_add_instruction(ctor_bc, OP_EQ, 0);
                int j_ok_present = bytecode_add_instruction(ctor_bc, OP_JUMP_IF_FALSE, 0);
                /* then -> error */
                {
                    char msg[128];
                    snprintf(msg, sizeof(msg), "TypeError: missing argument '%s' in %s()", param_names[i], cname);
                    int ci_msg = bytecode_add_constant(ctor_bc, make_string(msg));
                    bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_msg);
                    bytecode_add_instruction(ctor_bc, OP_PRINT, 0);
                    bytecode_add_instruction(ctor_bc, OP_HALT, 0);
                }
                /* patch continue if present */
                bytecode_set_operand(ctor_bc, j_ok_present, ctor_bc->instr_count);

                /* type check for known kinds */
                int kind = param_kind[i];
                if (kind == 1 || kind == 2 || kind == 3) {
                    /* typeof(local i) == Expected ? skip error : go to error */
                    bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, i);
                    bytecode_add_instruction(ctor_bc, OP_TYPEOF, 0);
                    const char *exp = (kind == 1) ? "Number" : (kind == 2) ? "String" : "Nil";
                    int ci_exp = bytecode_add_constant(ctor_bc, make_string(exp));
                    bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_exp);
                    bytecode_add_instruction(ctor_bc, OP_EQ, 0);
                    /* if false -> jump to error */
                    int j_to_error = bytecode_add_instruction(ctor_bc, OP_JUMP_IF_FALSE, 0);
                    /* on success, skip error block */
                    int j_skip_err = bytecode_add_instruction(ctor_bc, OP_JUMP, 0);
                    /* error block */
                    {
                        int err_label = ctor_bc->instr_count;
                        bytecode_set_operand(ctor_bc, j_to_error, err_label);
                        char msg2[160];
                        snprintf(msg2, sizeof(msg2), "TypeError: %s() expects %s for '%s'", cname, exp, param_names[i]);
                        int ci_msg2 = bytecode_add_constant(ctor_bc, make_string(msg2));
                        bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_msg2);
                        bytecode_add_instruction(ctor_bc, OP_PRINT, 0);
                        bytecode_add_instruction(ctor_bc, OP_HALT, 0);
                    }
                    /* continue label after error block */
                    bytecode_set_operand(ctor_bc, j_skip_err, ctor_bc->instr_count);
                }
            }

            /* Extra args check: guard local must be Nil; if not Nil -> error */
            {
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_extra);
                bytecode_add_instruction(ctor_bc, OP_TYPEOF, 0);
                int ci_nil2 = bytecode_add_constant(ctor_bc, make_string("Nil"));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_nil2);
                bytecode_add_instruction(ctor_bc, OP_EQ, 0);
                /* if false (not Nil) -> jump to error */
                int j_to_error = bytecode_add_instruction(ctor_bc, OP_JUMP_IF_FALSE, 0);
                /* if true (Nil) -> skip error */
                int j_skip_err = bytecode_add_instruction(ctor_bc, OP_JUMP, 0);
                {
                    int err_label = ctor_bc->instr_count;
                    bytecode_set_operand(ctor_bc, j_to_error, err_label);
                    char msg3[128];
                    snprintf(msg3, sizeof(msg3), "TypeError: %s() received too many arguments", cname);
                    int ci_msg3 = bytecode_add_constant(ctor_bc, make_string(msg3));
                    bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, ci_msg3);
                    bytecode_add_instruction(ctor_bc, OP_PRINT, 0);
                    bytecode_add_instruction(ctor_bc, OP_HALT, 0);
                }
                bytecode_set_operand(ctor_bc, j_skip_err, ctor_bc->instr_count);
            }

            /* instance map: __this = {} (placed after param guard) */
            int l_this = local_add("__this");
            bytecode_add_instruction(ctor_bc, OP_MAKE_MAP, 0);
            bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_this);

            /* tag instance with its class name: this["__class"] = "<ClassName>" */
            bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
            {
                int kci_cls = bytecode_add_constant(ctor_bc, make_string("__class"));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, kci_cls);
                int vci_cls = bytecode_add_constant(ctor_bc, make_string(cname));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, vci_cls);
            }
            bytecode_add_instruction(ctor_bc, OP_INDEX_SET, 0);

            /* Inheritance: if extends Parent, create Parent(header args...) and merge its keys into this */
            if (parent_name) {
                /* parent_inst = Parent(args...) */
                int parent_gi = sym_index(parent_name);
                bytecode_add_instruction(ctor_bc, OP_LOAD_GLOBAL, parent_gi);
                for (int i = 0; i < pcount; ++i) {
                    bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, i);
                }
                bytecode_add_instruction(ctor_bc, OP_CALL, pcount);
                int l_parent = local_add("__parent_inst");
                bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_parent);

                /* keys = keys(parent_inst) */
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_parent);
                bytecode_add_instruction(ctor_bc, OP_KEYS, 0);
                int l_keys = local_add("__parent_keys");
                bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_keys);

                /* i = 0 */
                int c0_inh = bytecode_add_constant(ctor_bc, make_int(0));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, c0_inh);
                int l_i = local_add("__inh_i");
                bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_i);

                /* loop: while (i < len(keys)) */
                int loop_start = ctor_bc->instr_count;
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_i);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_keys);
                bytecode_add_instruction(ctor_bc, OP_LEN, 0);
                bytecode_add_instruction(ctor_bc, OP_LT, 0);
                int jmp_false = bytecode_add_instruction(ctor_bc, OP_JUMP_IF_FALSE, 0);

                /* key = keys[i] */
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_keys);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_i);
                bytecode_add_instruction(ctor_bc, OP_INDEX_GET, 0);
                int l_k = local_add("__inh_k");
                bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_k);

                /* if this has key -> skip set */
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_k);
                bytecode_add_instruction(ctor_bc, OP_HAS_KEY, 0);
                int j_skip_set = bytecode_add_instruction(ctor_bc, OP_JUMP_IF_FALSE, 0);
                /* has key -> nothing to do, jump over set sequence */
                int j_after_maybe_set = bytecode_add_instruction(ctor_bc, OP_JUMP, 0);

                /* not has key: set this[key] = parent_inst[key] */
                bytecode_set_operand(ctor_bc, j_skip_set, ctor_bc->instr_count);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_k);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_parent);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_k);
                bytecode_add_instruction(ctor_bc, OP_INDEX_GET, 0);
                bytecode_add_instruction(ctor_bc, OP_INDEX_SET, 0);

                /* continue after maybe-set */
                bytecode_set_operand(ctor_bc, j_after_maybe_set, ctor_bc->instr_count);

                /* i++ */
                int c1_inh = bytecode_add_constant(ctor_bc, make_int(1));
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_i);
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, c1_inh);
                bytecode_add_instruction(ctor_bc, OP_ADD, 0);
                bytecode_add_instruction(ctor_bc, OP_STORE_LOCAL, l_i);

                /* back to loop */
                bytecode_add_instruction(ctor_bc, OP_JUMP, loop_start);
                /* end loop */
                bytecode_set_operand(ctor_bc, jmp_false, ctor_bc->instr_count);
            }

            /* Parse class body at increased indent */
            int body_indent = 0;
            size_t look_body = *pos;
            if (read_line_start(src, len, &look_body, &body_indent) && body_indent > current_indent) {
                /* iterate over class members at body_indent */
                for (;;) {
                    size_t member_line_start = *pos;
                    int member_indent = 0;
                    if (!read_line_start(src, len, pos, &member_indent)) {
                        /* EOF */
                        break;
                    }
                    if (member_indent < body_indent) {
                        /* end of class body */
                        *pos = member_line_start;
                        break;
                    }
                    if (member_indent > body_indent) {
                        /* skip nested blocks that are part of previous method parsing */
                        parse_block(ctor_bc, src, len, pos, member_indent);
                        continue;
                    }

                    /* at body_indent: member declaration (field = expr) or method 'fun name(...)' */
                    if (starts_with(src, len, *pos, "fun")) {
                        /* method definition: fun m(this, ...) ... */
                        *pos += 3;
                        skip_spaces(src, len, pos);
                        char *mname = NULL;
                        if (!read_identifier_into(src, len, pos, &mname)) {
                            parser_fail(*pos, "Expected method name after 'fun' in class");
                            g_locals = prev_env;
                            free(cname);
                            return;
                        }
                        int is_ctor_method = (strcmp(mname, "_construct") == 0);

                        skip_spaces(src, len, pos);
                        if (!consume_char(src, len, pos, '(')) {
                            parser_fail(*pos, "Expected '(' after method name");
                            free(mname);
                            g_locals = prev_env;
                            free(cname);
                            return;
                        }

                        /* Build method function bytecode */
                        Bytecode *m_bc = bytecode_new();
                                                if (m_bc) {
                                                    if (m_bc->name) free((void*)m_bc->name);
                                                    /* method qualified name: Class.method */
                                                    size_t qlen = strlen(cname) + 1 + strlen(mname) + 1;
                                                    char *q = (char*)malloc(qlen);
                                                    if (q) {
                                                        snprintf(q, qlen, "%s.%s", cname, mname);
                                                        m_bc->name = q;
                                                    }
                                                    if (m_bc->source_file) free((void*)m_bc->source_file);
                                                    if (g_current_source_path) m_bc->source_file = strdup(g_current_source_path);
                                                }
                        LocalEnv m_env;
                        memset(&m_env, 0, sizeof(m_env));
                        LocalEnv *saved = g_locals;
                        g_locals = &m_env;

                        /* Parse params, ensure first is 'this' (insert if missing) */
                        int saw_param = 0;
                        int param_count = 0;
                        skip_spaces(src, len, pos);
                        if (*pos < len && src[*pos] != ')') {
                            for (;;) {
                                char *pname = NULL;
                                if (!read_identifier_into(src, len, pos, &pname)) {
                                    parser_fail(*pos, "Expected parameter name");
                                    free(mname);
                                    g_locals = saved;
                                    g_locals = prev_env;
                                    free(cname);
                                    return;
                                }
                                if (param_count == 0 && strcmp(pname, "this") != 0) {
                                    /* Require explicit 'this' as first parameter */
                                    if (is_ctor_method) {
                                        parser_fail(*pos, "Constructor '_construct' must declare 'this' as its first parameter");
                                    } else {
                                        parser_fail(*pos, "First parameter of a method must be 'this'");
                                    }
                                    free(pname);
                                    free(mname);
                                    g_locals = saved;
                                    g_locals = prev_env;
                                    free(cname);
                                    return;
                                }
                                local_add(pname);
                                free(pname);
                                param_count++;
                                skip_spaces(src, len, pos);
                                if (*pos < len && src[*pos] == ',') {
                                    (*pos)++;
                                    skip_spaces(src, len, pos);
                                    continue;
                                }
                                break;
                            }
                        } else {
                            /* no params: enforce (this) */
                            if (is_ctor_method) {
                                parser_fail(*pos, "Constructor '_construct' must declare 'this' as its first parameter");
                            } else {
                                parser_fail(*pos, "Method must declare at least 'this' parameter");
                            }
                            free(mname);
                            g_locals = saved;
                            g_locals = prev_env;
                            free(cname);
                            return;
                        }

                        if (!consume_char(src, len, pos, ')')) {
                            parser_fail(*pos, "Expected ')' after method parameter list");
                            free(mname);
                            g_locals = saved;
                            g_locals = prev_env;
                            free(cname);
                            return;
                        }
                        /* end header line */
                        skip_to_eol(src, len, pos);

                        /* parse method body at increased indent */
                        int m_body_indent = 0;
                        size_t look_m = *pos;
                        if (read_line_start(src, len, &look_m, &m_body_indent) && m_body_indent > body_indent) {
                            parse_block(m_bc, src, len, pos, m_body_indent);
                        } else {
                            /* empty method body allowed -> return */
                        }
                        /* ensure return */
                        bytecode_add_instruction(m_bc, OP_RETURN, 0);

                        /* restore env to factory */
                        g_locals = saved;

                        /* Insert method function into instance: this["mname"] = <function> */
                        bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                        int kci = bytecode_add_constant(ctor_bc, make_string(mname));
                        bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, kci);
                        int mci = bytecode_add_constant(ctor_bc, make_function(m_bc));
                        bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, mci);
                        bytecode_add_instruction(ctor_bc, OP_INDEX_SET, 0);

                        /* mark constructor presence if name matches */
                        if (strcmp(mname, "_construct") == 0) {
                            ctor_present = 1;
                        }

                        free(mname);
                        continue;
                    }

                    /* field initializer: ident = expr */
                    size_t lp = *pos;
                    char *fname = NULL;
                    if (!read_identifier_into(src, len, &lp, &fname)) {
                        parser_fail(*pos, "Expected field or 'fun' in class body");
                        g_locals = prev_env;
                        free(cname);
                        return;
                    }
                    size_t tmp = lp;
                    skip_spaces(src, len, &tmp);
                    if (tmp >= len || src[tmp] != '=') {
                        free(fname);
                        parser_fail(tmp, "Expected '=' in field initializer");
                        g_locals = prev_env;
                        free(cname);
                        return;
                    }
                    /* commit position and consume '=' */
                    *pos = tmp + 1;
                    /* emit: this["fname"] = (expr) */
                    bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                    int fkey = bytecode_add_constant(ctor_bc, make_string(fname));
                    bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, fkey);
                    free(fname);
                    if (!emit_expression(ctor_bc, src, len, pos)) {
                        parser_fail(*pos, "Expected expression in field initializer");
                        g_locals = prev_env;
                        free(cname);
                        return;
                    }
                    bytecode_add_instruction(ctor_bc, OP_INDEX_SET, 0);
                    /* end of line */
                    skip_to_eol(src, len, pos);
                }
            } else {
                /* empty class body allowed */
            }

            /* Override defaults with constructor parameters: this["name"] = local i */
            for (int i = 0; i < pcount; ++i) {
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                int kci = bytecode_add_constant(ctor_bc, make_string(param_names[i]));
                bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, kci);
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, i);
                bytecode_add_instruction(ctor_bc, OP_INDEX_SET, 0);
            }

            /* If a constructor exists, invoke: this._construct(this, params...) and drop its return */
            if (ctor_present) {
                /* fetch method: duplicate 'this' so we keep it for the call */
                bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
                bytecode_add_instruction(ctor_bc, OP_DUP, 0);       /* -> this, this */
                {
                    int kci_ctor = bytecode_add_constant(ctor_bc, make_string("_construct"));
                    bytecode_add_instruction(ctor_bc, OP_LOAD_CONST, kci_ctor);
                }
                bytecode_add_instruction(ctor_bc, OP_INDEX_GET, 0); /* -> this, func */
                bytecode_add_instruction(ctor_bc, OP_SWAP, 0);      /* -> func, this */

                /* push all header parameters as additional args in order */
                for (int i = 0; i < pcount; ++i) {
                    bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, i);
                }
                /* call with implicit 'this' (+1) plus pcount params */
                bytecode_add_instruction(ctor_bc, OP_CALL, pcount + 1);
                /* discard any return value from constructor */
                bytecode_add_instruction(ctor_bc, OP_POP, 0);
            }

            /* return instance */
            bytecode_add_instruction(ctor_bc, OP_LOAD_LOCAL, l_this);
            bytecode_add_instruction(ctor_bc, OP_RETURN, 0);

            /* restore outer locals env */
            g_locals = prev_env;

            /* bind factory function globally under class name */
            int cci = bytecode_add_constant(bc, make_function(ctor_bc));
            bytecode_add_instruction(bc, OP_LOAD_CONST, cci);
            bytecode_add_instruction(bc, OP_STORE_GLOBAL, cgi);
            /* mark this global as a class for typeof(identifier) */
            G.is_class[cgi] = 1;

            for (int i = 0; i < pcount; ++i) free(param_names[i]);
            if (parent_name) free(parent_name);
            free(cname);
            continue;
        }

        if (starts_with(src, len, *pos, "fun")) {
            /* parse header: fun name(arg, ...) */
            *pos += 3;
            skip_spaces(src, len, pos);
            char *fname = NULL;
            if (!read_identifier_into(src, len, pos, &fname)) {
                parser_fail(*pos, "Expected function name after 'fun'");
                return;
            }
            int fgi = sym_index(fname);
            skip_spaces(src, len, pos);
            if (!consume_char(src, len, pos, '(')) {
                parser_fail(*pos, "Expected '(' after function name");
                free(fname);
                return;
            }

            /* build locals from parameters */
            LocalEnv env = { {0}, 0 };
            LocalEnv *prev = g_locals;
            g_locals = &env;

            skip_spaces(src, len, pos);
            if (*pos < len && src[*pos] != ')') {
                for (;;) {
                    char *pname = NULL;
                    if (!read_identifier_into(src, len, pos, &pname)) {
                        parser_fail(*pos, "Expected parameter name");
                        g_locals = prev;
                        free(fname);
                        return;
                    }
                    if (local_find(pname) >= 0) {
                        parser_fail(*pos, "Duplicate parameter name '%s'", pname);
                        free(pname);
                        g_locals = prev;
                        free(fname);
                        return;
                    }
                    local_add(pname);
                    free(pname);
                    skip_spaces(src, len, pos);
                    if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); continue; }
                    break;
                }
            }
            if (!consume_char(src, len, pos, ')')) {
                parser_fail(*pos, "Expected ')' after parameter list");
                g_locals = prev;
                free(fname);
                return;
            }
            /* end header line */
            skip_to_eol(src, len, pos);

            /* compile body into separate Bytecode */
            Bytecode *fn_bc = bytecode_new();
                        if (fn_bc) {
                            if (fn_bc->name) free((void*)fn_bc->name);
                            fn_bc->name = strdup(fname);
                            if (fn_bc->source_file) free((void*)fn_bc->source_file);
                            if (g_current_source_path) fn_bc->source_file = strdup(g_current_source_path);
                        }

            /* parse body at increased indent if present */
            int body_indent = 0;
            size_t look_body = *pos;
            if (read_line_start(src, len, &look_body, &body_indent) && body_indent > current_indent) {
                /* do not advance pos here; let parse_block consume the line */
                parse_block(fn_bc, src, len, pos, body_indent);
            } else {
                /* empty body: ok */
            }
            /* ensure function returns */
            bytecode_add_instruction(fn_bc, OP_RETURN, 0);

#ifdef FUN_DEBUG
            /* DEBUG: dump compiled function bytecode */
            printf("=== compiled function %s (%d params) ===\n", fname, env.count);
            bytecode_dump(fn_bc);
            printf("=== end function %s ===\n", fname);
#endif

            /* bind function to global: LOAD_CONST <fn> ; STORE_GLOBAL fgi */
            int fci = bytecode_add_constant(bc, make_function(fn_bc));
            bytecode_add_instruction(bc, OP_LOAD_CONST, fci);
            bytecode_add_instruction(bc, OP_STORE_GLOBAL, fgi);

            g_locals = prev;
            free(fname);
            continue;
        }

        /* for-sugar:
         *   - for <ident> in range(a, b)
         *   - for <ident> in <array-expr>
         */
        if (starts_with(src, len, *pos, "for")) {
            *pos += 3;
            skip_spaces(src, len, pos);

            /* loop variable name */
            char *ivar = NULL;
            if (!read_identifier_into(src, len, pos, &ivar)) {
                parser_fail(*pos, "Expected loop variable after 'for'");
                return;
            }

            skip_spaces(src, len, pos);
            if (!starts_with(src, len, *pos, "in")) {
                parser_fail(*pos, "Expected 'in' after loop variable");
                free(ivar);
                return;
            }
            *pos += 2;
            skip_spaces(src, len, pos);

            if (starts_with(src, len, *pos, "range")) {
                /* ===== range(a, b) variant ===== */
                *pos += 5;
                if (!consume_char(src, len, pos, '(')) {
                    parser_fail(*pos, "Expected '(' after range");
                    free(ivar);
                    return;
                }

                /* Parse start expression */
                if (!emit_expression(bc, src, len, pos)) {
                    parser_fail(*pos, "Expected start expression in range");
                    free(ivar);
                    return;
                }

                /* Determine loop variable storage and store start value */
                int lidx = local_find(ivar);
                int gi = -1;
                if (lidx < 0) {
                    if (g_locals) lidx = local_add(ivar);
                    else gi = sym_index(ivar);
                }
                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }

                /* comma */
                skip_spaces(src, len, pos);
                if (*pos >= len || src[*pos] != ',') {
                    parser_fail(*pos, "Expected ',' between range start and end");
                    free(ivar);
                    return;
                }
                (*pos)++; /* consume ',' */
                skip_spaces(src, len, pos);

                /* Parse end expression and store in a temp (local or global) */
                if (!emit_expression(bc, src, len, pos)) {
                    parser_fail(*pos, "Expected end expression in range");
                    free(ivar);
                    return;
                }

                char tmpname[64];
                snprintf(tmpname, sizeof(tmpname), "__for_end_%d", g_temp_counter++);

                int lend = -1, gend = -1;
                if (g_locals) {
                    lend = local_add(tmpname);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lend);
                } else {
                    gend = sym_index(tmpname);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gend);
                }

                if (!consume_char(src, len, pos, ')')) {
                    parser_fail(*pos, "Expected ')' after range arguments");
                    free(ivar);
                    return;
                }

                /* end of header line */
                skip_to_eol(src, len, pos);

                /* emit loop */
                int loop_start = bc->instr_count;

                /* condition: ivar < end_tmp */
                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
                }
                if (lend >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, lend);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gend);
                }
                bytecode_add_instruction(bc, OP_LT, 0);
                int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

                /* enter loop context for break/continue */
                LoopCtx ctx = { {0}, 0, {0}, 0, g_loop_ctx };
                g_loop_ctx = &ctx;

                /* parse body at increased indent (peek) */
                int body_indent = 0;
                size_t look_body = *pos;
                if (read_line_start(src, len, &look_body, &body_indent) && body_indent > current_indent) {
                    parse_block(bc, src, len, pos, body_indent);
                } else {
                    /* empty body ok */
                }

                /* continue target: start of increment */
                int cont_label = bc->instr_count;

                /* i = i + 1 */
                int c1 = bytecode_add_constant(bc, make_int(1));
                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, lidx);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
                    bytecode_add_instruction(bc, OP_ADD, 0);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
                    bytecode_add_instruction(bc, OP_ADD, 0);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }

                /* back edge and patch */
                bytecode_add_instruction(bc, OP_JUMP, loop_start);

                /* end label (after loop) */
                int end_label = bc->instr_count;
                bytecode_set_operand(bc, jmp_false, end_label);

                /* patch continue/break jumps */
                for (int bi = 0; bi < ctx.cont_count; ++bi) {
                    bytecode_set_operand(bc, ctx.continue_jumps[bi], cont_label);
                }
                for (int bi = 0; bi < ctx.break_count; ++bi) {
                    bytecode_set_operand(bc, ctx.break_jumps[bi], end_label);
                }
                g_loop_ctx = ctx.prev;

                free(ivar);
                continue;
            } else {
                /* ===== array iteration: for ivar in <expr> ===== */
                /* Evaluate the iterable once and store in a temp */
                if (!emit_expression(bc, src, len, pos)) {
                    parser_fail(*pos, "Expected iterable expression after 'in'");
                    free(ivar);
                    return;
                }
                char arrname[64];
                snprintf(arrname, sizeof(arrname), "__for_arr_%d", g_temp_counter++);
                int larr = -1, garr = -1;
                if (g_locals) {
                    larr = local_add(arrname);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, larr);
                } else {
                    garr = sym_index(arrname);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, garr);
                }

                /* Compute length once: len(arr) -> store temp */
                if (larr >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr);
                }
                bytecode_add_instruction(bc, OP_LEN, 0);
                char lenname[64];
                snprintf(lenname, sizeof(lenname), "__for_len_%d", g_temp_counter++);
                int llen = -1, glen = -1;
                if (g_locals) {
                    llen = local_add(lenname);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, llen);
                } else {
                    glen = sym_index(lenname);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, glen);
                }

                /* Index temp: i = 0 */
                int c0 = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, c0);
                char iname[64];
                snprintf(iname, sizeof(iname), "__for_i_%d", g_temp_counter++);
                int li = -1, gi = -1;
                if (g_locals) {
                    li = local_add(iname);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, li);
                } else {
                    gi = sym_index(iname);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }

                /* end of header line */
                skip_to_eol(src, len, pos);

                /* loop start label */
                int loop_start = bc->instr_count;

                /* condition: i < len */
                if (li >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, li);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
                }
                if (llen >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, llen);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, glen);
                }
                bytecode_add_instruction(bc, OP_LT, 0);
                int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

                /* element: ivar = arr[i] */
                if (larr >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, larr);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, garr);
                }
                if (li >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, li);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
                }
                bytecode_add_instruction(bc, OP_INDEX_GET, 0);

                /* assign to ivar (local preferred) */
                int ldst = local_find(ivar);
                int gdst = -1;
                if (ldst < 0) {
                    if (g_locals) ldst = local_add(ivar);
                    else gdst = sym_index(ivar);
                }
                if (ldst >= 0) {
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, ldst);
                } else {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gdst);
                }

                /* enter loop context for break/continue */
                LoopCtx ctx = { {0}, 0, {0}, 0, g_loop_ctx };
                g_loop_ctx = &ctx;

                /* parse body at increased indent */
                int body_indent = 0;
                size_t look_body = *pos;
                if (read_line_start(src, len, &look_body, &body_indent) && body_indent > current_indent) {
                    parse_block(bc, src, len, pos, body_indent);
                } else {
                    /* empty body ok */
                }

                /* continue target: start of increment */
                int cont_label = bc->instr_count;

                /* i = i + 1 */
                int c1 = bytecode_add_constant(bc, make_int(1));
                if (li >= 0) {
                    bytecode_add_instruction(bc, OP_LOAD_LOCAL, li);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
                    bytecode_add_instruction(bc, OP_ADD, 0);
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, li);
                } else {
                    bytecode_add_instruction(bc, OP_LOAD_GLOBAL, gi);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
                    bytecode_add_instruction(bc, OP_ADD, 0);
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                }

                /* back edge and patch */
                bytecode_add_instruction(bc, OP_JUMP, loop_start);

                /* end label (after loop) */
                int end_label = bc->instr_count;
                bytecode_set_operand(bc, jmp_false, end_label);

                /* patch continue/break jumps */
                for (int bi = 0; bi < ctx.cont_count; ++bi) {
                    bytecode_set_operand(bc, ctx.continue_jumps[bi], cont_label);
                }
                for (int bi = 0; bi < ctx.break_count; ++bi) {
                    bytecode_set_operand(bc, ctx.break_jumps[bi], end_label);
                }
                g_loop_ctx = ctx.prev;

                free(ivar);
                continue;
            }
        }

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

                /* Decide between inline single-statement and indented block on next line */
                size_t ppeek = *pos;
                /* skip spaces after condition */
                while (ppeek < len && src[ppeek] == ' ') ppeek++;
                int inline_stmt = 0;
                if (ppeek < len) {
                    if (src[ppeek] == '\r' || src[ppeek] == '\n') {
                        inline_stmt = 0; /* EOL -> no inline body */
                    } else if (ppeek + 1 < len && src[ppeek] == '/' && src[ppeek + 1] == '/') {
                        inline_stmt = 0; /* line comment -> no inline body */
                    } else if (ppeek + 1 < len && src[ppeek] == '/' && src[ppeek + 1] == '*') {
                        inline_stmt = 0; /* block comment at EOL -> no inline body */
                    } else {
                        inline_stmt = 1; /* there's code after condition on same line */
                    }
                }

                /* conditional jump over this clause's inline/body */
                int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

                if (inline_stmt) {
                    /* Compile a single inline statement on the same line:
                       if (cond) <statement> */
                    *pos = ppeek;
                    parse_simple_statement(bc, src, len, pos);
                    /* Skip the inline body when condition is false */
                    bytecode_set_operand(bc, jmp_false, bc->instr_count);
                    /* one-liner form has no else/elseif on the same line; end the chain */
                    break;
                }

                /* No inline statement on this line: consume up to EOL and parse indented block or else-if/else */
                skip_to_eol(src, len, pos);

                /* parse nested block if next line is indented */
                int next_indent = 0;
                size_t look_next = *pos;
                if (read_line_start(src, len, &look_next, &next_indent)) {
                    if (next_indent > current_indent) {
                        /* parse body at increased indent (let parse_block consume the line) */
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
                        size_t look_else = *pos;
                        if (read_line_start(src, len, &look_else, &else_indent) && else_indent > current_indent) {
                            /* let parse_block consume the line */
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

        /* while loop */
        if (starts_with(src, len, *pos, "while")) {
            *pos += 5;
            skip_spaces(src, len, pos);

            int loop_start = bc->instr_count;

            /* condition */
            if (!emit_expression(bc, src, len, pos)) {
                int ci = bytecode_add_constant(bc, make_int(0));
                bytecode_add_instruction(bc, OP_LOAD_CONST, ci);
            }
            /* end of condition */
            skip_to_eol(src, len, pos);

            /* jump over body if false */
            int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

            /* enter loop context (continue -> condition) */
            LoopCtx ctx = { {0}, 0, {0}, 0, g_loop_ctx };
            g_loop_ctx = &ctx;

            /* parse body at increased indent (peek indent without advancing pos) */
            int body_indent = 0;
            size_t look_body = *pos;
            if (read_line_start(src, len, &look_body, &body_indent) && body_indent > current_indent) {
                parse_block(bc, src, len, pos, body_indent);
            } else {
                /* empty body allowed */
            }

            /* patch pending continues to loop_start (re-evaluate condition) */
            for (int bi = 0; bi < ctx.cont_count; ++bi) {
                bytecode_set_operand(bc, ctx.continue_jumps[bi], loop_start);
            }

            /* back edge to loop start */
            bytecode_add_instruction(bc, OP_JUMP, loop_start);

            /* end label and patches */
            int end_label = bc->instr_count;
            bytecode_set_operand(bc, jmp_false, end_label);
            for (int bi = 0; bi < ctx.break_count; ++bi) {
                bytecode_set_operand(bc, ctx.break_jumps[bi], end_label);
            }
            g_loop_ctx = ctx.prev;
            continue;
        }

        /* try/catch/finally */
        if (starts_with(src, len, *pos, "try")) {
            /* consume 'try' */
            *pos += 3;
            /* end of header line */
            skip_to_eol(src, len, pos);

            /* Install a handler placeholder; will be patched to catch label (or a rethrow stub) */
            int try_push_idx = bytecode_add_instruction(bc, OP_TRY_PUSH, 0);

            /* parse try body at increased indent (if any) */
            int try_body_indent = 0;
            size_t look_try = *pos;
            if (read_line_start(src, len, &look_try, &try_body_indent) && try_body_indent > current_indent) {
                parse_block(bc, src, len, pos, try_body_indent);
            } else {
                /* empty try body allowed */
            }

            /* After try body, pop handler for normal (non-exceptional) flow */
            bytecode_add_instruction(bc, OP_TRY_POP, 0);

            /* on normal completion, jump over catch body */
            int jmp_over_catch_finally = bytecode_add_instruction(bc, OP_JUMP, 0);

            /* Optional: catch and/or finally clauses at same indentation */
            int seen_catch = 0;
            int seen_finally = 0;
            int catch_label = -1;
            for (;;) {
                size_t look = *pos;
                int look_indent = 0;
                if (!read_line_start(src, len, &look, &look_indent)) break; /* EOF */
                if (look_indent != current_indent) break; /* different indentation -> stop */

                if (!seen_catch && starts_with(src, len, look, "catch")) {
                    /* consume 'catch' */
                    *pos = look + 5;
                    /* optional variable name */
                    skip_spaces(src, len, pos);
                    char *ex_name = NULL;
                    size_t tmp = *pos;
                    int have_name = 0;
                    if (read_identifier_into(src, len, &tmp, &ex_name)) {
                        *pos = tmp;
                        have_name = 1;
                    }
                    /* end of header line */
                    skip_to_eol(src, len, pos);

                    /* Mark catch label and patch try handler target */
                    catch_label = bc->instr_count;
                    bytecode_set_operand(bc, try_push_idx, catch_label);

                    /* On entering catch, the thrown error is on stack. Bind to name if provided, else pop. */
                    if (have_name) {
                        int lidx = -1, gi = -1;
                        if (g_locals) {
                            int existing = local_find(ex_name);
                            if (existing >= 0) lidx = existing; else lidx = local_add(ex_name);
                        } else {
                            gi = sym_index(ex_name);
                        }
                        if (lidx >= 0) bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                        else if (gi >= 0) bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
                        else bytecode_add_instruction(bc, OP_POP, 0);
                    } else {
                        bytecode_add_instruction(bc, OP_POP, 0);
                    }
                    if (ex_name) free(ex_name);

                    /* parse catch body at increased indent (if any) */
                    int catch_indent = 0;
                    size_t look_catch = *pos;
                    if (read_line_start(src, len, &look_catch, &catch_indent) && catch_indent > current_indent) {
                        parse_block(bc, src, len, pos, catch_indent);
                    } else {
                        /* empty catch body allowed */
                    }
                    seen_catch = 1;
                    continue;
                }

                if (!seen_finally && starts_with(src, len, look, "finally")) {
                    /* consume 'finally' */
                    *pos = look + 7;
                    /* end of header line */
                    skip_to_eol(src, len, pos);

                    /* parse finally body at increased indent (if any) */
                    int finally_indent = 0;
                    size_t look_fin = *pos;
                    if (read_line_start(src, len, &look_fin, &finally_indent) && finally_indent > current_indent) {
                        parse_block(bc, src, len, pos, finally_indent);
                    } else {
                        /* empty finally body allowed */
                    }

                    seen_finally = 1;
                    continue;
                }

                /* no recognized clause at this indentation */
                break;
            }

            /* If no catch clause was present, make handler rethrow */
            if (!seen_catch) {
                int rethrow_label = bc->instr_count;
                bytecode_set_operand(bc, try_push_idx, rethrow_label);
                /* at handler: immediately rethrow the incoming error */
                bytecode_add_instruction(bc, OP_THROW, 0);
            }

            /* patch normal-flow jump to here (after catch/finally) */
            bytecode_set_operand(bc, jmp_over_catch_finally, bc->instr_count);
            continue;
        }

        /* otherwise: simple statement on this line */
        parse_simple_statement(bc, src, len, pos);
    }
}

static Bytecode *compile_minimal(const char *src, size_t len) {
    Bytecode *bc = bytecode_new();
    size_t pos = 0;

    /* Refresh namespace alias table for this compilation unit */
    ns_aliases_reset();
    ns_aliases_scan(src, len);

    skip_shebang_if_present(src, len, &pos);

    /* Allow top-of-file function definitions; do not skip any leading 'fun' line */
    skip_comments(src, len, &pos);
    skip_ws(src, len, &pos);

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

    /* Preprocess includes before compiling */
    char *prep = preprocess_includes(src);
    const char *compile_src = prep ? prep : src;
    size_t compile_len = strlen(compile_src);

    /* reset error state */
    g_has_error = 0;
    g_err_pos = 0;
    g_err_msg[0] = '\0';
    g_err_line = 0;
    g_err_col  = 0;

    /* Set current source path for nested bytecodes to inherit */
        const char *prev_source = g_current_source_path;
        g_current_source_path = path;
        Bytecode *bc = compile_minimal(compile_src, compile_len);
        /* assign debug metadata to module bytecode */
        if (bc) {
            if (bc->source_file) free((void*)bc->source_file);
            bc->source_file = path ? strdup(path) : strdup("<input>");
            if (bc->name) free((void*)bc->name);
            /* derive name from basename of path */
            const char *bn = path ? strrchr(path, '/') : NULL;
            const char *base = bn ? bn + 1 : (path ? path : "<input>");
            bc->name = strdup(base);
        }
        /* restore previous */
        g_current_source_path = prev_source;

    if (g_has_error) {
        int line = 1, col = 1;
        calc_line_col(compile_src, compile_len, g_err_pos, &line, &col);
        g_err_line = line;
        g_err_col  = col;

        /* Try to locate include context marker preceding error */
        const char *marker = "// __include_begin__: ";
        size_t mlen = strlen(marker);
        int inner_line = -1;
        char inc_path[512]; inc_path[0] = '\0';
        /* scan backward to find last marker line */
        size_t scan = g_err_pos;
        while (scan > 0) {
            /* find start of current line */
            size_t ls = scan;
            while (ls > 0 && compile_src[ls - 1] != '\n') ls--;
            /* check if this line starts with marker */
            if (ls + mlen <= compile_len && strncmp(compile_src + ls, marker, mlen) == 0) {
                /* extract path until end-of-line */
                size_t p = ls + mlen;
                size_t pe = p;
                while (pe < compile_len && compile_src[pe] != '\n' && (pe - p) < sizeof(inc_path) - 1) pe++;
                memcpy(inc_path, compile_src + p, pe - p);
                inc_path[pe - p] = '\0';
                /* compute inner line as number of newlines from (pe+1) to error position */
                int count = 1;
                size_t q = (pe < compile_len && compile_src[pe] == '\n') ? (pe + 1) : pe;
                while (q < g_err_pos) {
                    if (compile_src[q] == '\n') count++;
                    q++;
                }
                inner_line = count;
                break;
            }
            /* move to previous line */
            if (ls == 0) break;
            scan = ls - 1;
        }

        if (inner_line > 0 && inc_path[0] != '\0') {
            fprintf(stderr, "Parse error %s:%d:%d: %s (in %s:%d)\n",
                    path ? path : "<input>", line, col, g_err_msg, inc_path, inner_line);
        } else {
            fprintf(stderr, "Parse error %s:%d:%d: %s\n", path ? path : "<input>", line, col, g_err_msg);
        }

        if (bc) bytecode_free(bc);
        if (prep) free(prep);
        free(src);
        return NULL;
    }

    if (prep) free(prep);
    free(src);
    return bc;
}

Bytecode *parse_string_to_bytecode(const char *source) {
    if (!source) {
        fprintf(stderr, "Error: null source provided\n");
        return NULL;
    }

    /* Preprocess includes before compiling */
    char *prep = preprocess_includes(source);
    const char *compile_src = prep ? prep : source;
    size_t len = strlen(compile_src);

    /* reset error state */
    g_has_error = 0;
    g_err_pos = 0;
    g_err_msg[0] = '\0';
    g_err_line = 0;
    g_err_col  = 0;

    /* Set current source path to <input> for nested bytecodes */
    const char *prev_src = g_current_source_path;
    g_current_source_path = NULL;
    Bytecode *bc = compile_minimal(compile_src, len);
    if (bc) {
        if (bc->source_file) free((void*)bc->source_file);
        bc->source_file = strdup("<input>");
        if (bc->name) free((void*)bc->name);
        bc->name = strdup("<input>");
    }
    g_current_source_path = prev_src;

    if (g_has_error) {
        int line = 1, col = 1;
        calc_line_col(compile_src, len, g_err_pos, &line, &col);
        g_err_line = line;
        g_err_col  = col;
        if (bc) bytecode_free(bc);
        if (prep) free(prep);
        return NULL;
    }
    if (prep) free(prep);
    return bc;
}

int parser_last_error(char *msgBuf, unsigned long msgCap, int *outLine, int *outCol) {
    if (!g_has_error) return 0;
    if (msgBuf && msgCap > 0) {
        snprintf(msgBuf, msgCap, "%s", g_err_msg);
    }
    if (outLine) *outLine = g_err_line;
    if (outCol)  *outCol  = g_err_col;
    return 1;
}
