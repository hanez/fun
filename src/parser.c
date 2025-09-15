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
static int g_err_line = 0;
static int g_err_col  = 0;

/* ---- compiler-generated temporary counter ---- */
static int g_temp_counter = 0;

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

#include "parser_utils.c"

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

/* ---- locals environment for functions ---- */
typedef struct {
    char *names[FRAME_MAX_LOCALS];
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
    if (g_locals->count >= FRAME_MAX_LOCALS) {
        parser_fail(0, "Too many local variables/parameters (max %d)", FRAME_MAX_LOCALS);
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

    /* number */
    int ok = 0;
    size_t save = *pos;
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
                bytecode_add_instruction(bc, OP_ARR_PUSH, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "pop") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "pop expects array"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after pop arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ARR_POP, 0);
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
                bytecode_add_instruction(bc, OP_ARR_SET, 0);
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
                bytecode_add_instruction(bc, OP_ARR_INSERT, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "remove") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "remove expects array"); free(name); return 0; }
                if (*pos < len && src[*pos] == ',') { (*pos)++; skip_spaces(src, len, pos); } else { parser_fail(*pos, "remove expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos)) { parser_fail(*pos, "remove expects index"); free(name); return 0; }
                if (!consume_char(src, len, pos, ')')) { parser_fail(*pos, "Expected ')' after remove args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_ARR_REMOVE, 0);
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
                bytecode_add_instruction(bc, OP_ARR_INSERT, 0);
                /* discard returned new length */
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
                bytecode_add_instruction(bc, OP_ARR_INSERT, 0);
                /* discard returned new length */
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
            if (strcmp(name, "pow") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "pow expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "pow expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_POW, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "random") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "random expects 1 arg"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RANDOM_SEED, 0);
                free(name);
                return 1;
            }
            if (strcmp(name, "randomInt") == 0) {
                (*pos)++; /* '(' */
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ',')) { parser_fail(*pos, "randomInt expects 2 args"); free(name); return 0; }
                if (!emit_expression(bc, src, len, pos) || !consume_char(src, len, pos, ')')) { parser_fail(*pos, "randomInt expects 2 args"); free(name); return 0; }
                bytecode_add_instruction(bc, OP_RANDOM_INT, 0);
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

                /* dot property access: obj.field -> obj["field"] */
                if (*pos < len && src[*pos] == '.') {
                    (*pos)++; /* '.' */
                    skip_spaces(src, len, pos);
                    char *mname = NULL;
                    if (!read_identifier_into(src, len, pos, &mname)) { parser_fail(*pos, "Expected identifier after '.'"); free(name); return 0; }
                    int kci = bytecode_add_constant(bc, make_string(mname));
                    free(mname);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
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

                /* dot property access: obj.field -> obj["field"] */
                if (*pos < len && src[*pos] == '.') {
                    (*pos)++;
                    skip_spaces(src, len, pos);
                    char *mname = NULL;
                    if (!read_identifier_into(src, len, pos, &mname)) { parser_fail(*pos, "Expected identifier after '.'"); free(name); return 0; }
                    int kci = bytecode_add_constant(bc, make_string(mname));
                    free(mname);
                    bytecode_add_instruction(bc, OP_LOAD_CONST, kci);
                    bytecode_add_instruction(bc, OP_INDEX_GET, 0);
                    continue;
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
        int c1 = bytecode_add_constant(bc, make_int(1));
        bytecode_add_instruction(bc, OP_LOAD_CONST, c1);
        int j_end = bytecode_add_instruction(bc, OP_JUMP, 0);

        /* false label: patch all false jumps here, result false */
        int l_false = bc->instr_count;
        for (int i = 0; i < jf_count; ++i) {
            bytecode_set_operand(bc, jf_idxs[i], l_false);
        }
        int c0 = bytecode_add_constant(bc, make_int(0));
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

        /* true path: push 1 and jump to end */
        int c1 = bytecode_add_constant(bc, make_int(1));
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

        /* // comment-only line */
        if (p + 1 < len && src[p] == '/' && src[p + 1] == '/') {
            /* skip entire line */
            while (p < len && src[p] != '\n') p++;
            if (p < len && src[p] == '\n') p++;
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

            /* decide local vs global */
            int lidx = -1;
            int gi = -1;
            if (g_locals) {
                int existing = local_find(varname);
                if (existing >= 0) lidx = existing;
                else lidx = local_add(varname);
            } else {
                gi = sym_index(varname);
            }
            free(varname);

            skip_spaces(src, len, &local_pos);
            if (local_pos < len && src[local_pos] == '=') {
                local_pos++; /* '=' */
                if (!emit_expression(bc, src, len, &local_pos)) {
                    parser_fail(local_pos, "Expected initializer expression after '='");
                    return;
                }
                if (lidx >= 0) {
                    bytecode_add_instruction(bc, OP_STORE_LOCAL, lidx);
                } else {
                    bytecode_add_instruction(bc, OP_STORE_GLOBAL, gi);
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

        /* array element assignment: name[expr] = expr */
        if (local_pos < len && src[local_pos] == '[') {
            /* load array variable */
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
                bytecode_add_instruction(bc, OP_POP, 0); /* discard return value */
            }
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
                /* end of condition: ignore any trailing until EOL */
                skip_to_eol(src, len, pos);

                /* conditional jump over this clause's block */
                int jmp_false = bytecode_add_instruction(bc, OP_JUMP_IF_FALSE, 0);

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
    g_err_line = 0;
    g_err_col  = 0;

    Bytecode *bc = compile_minimal(src, len);

    if (g_has_error) {
        int line = 1, col = 1;
        calc_line_col(src, len, g_err_pos, &line, &col);
        g_err_line = line;
        g_err_col  = col;
        fprintf(stderr, "Parse error %s:%d:%d: %s\n", path ? path : "<input>", line, col, g_err_msg);
        if (bc) bytecode_free(bc);
        free(src);
        return NULL;
    }

    free(src);
    return bc;
}

Bytecode *parse_string_to_bytecode(const char *source) {
    if (!source) {
        fprintf(stderr, "Error: null source provided\n");
        return NULL;
    }
    size_t len = strlen(source);

    /* reset error state */
    g_has_error = 0;
    g_err_pos = 0;
    g_err_msg[0] = '\0';
    g_err_line = 0;
    g_err_col  = 0;

    Bytecode *bc = compile_minimal(source, len);

    if (g_has_error) {
        int line = 1, col = 1;
        calc_line_col(source, len, g_err_pos, &line, &col);
        g_err_line = line;
        g_err_col  = col;
        if (bc) bytecode_free(bc);
        return NULL;
    }
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