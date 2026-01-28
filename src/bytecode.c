/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>

Bytecode *bytecode_new(void) {
    Bytecode *bc = (Bytecode*)malloc(sizeof(Bytecode));
    bc->instructions = NULL;
    bc->instr_count = 0;
    bc->constants = NULL;
    bc->const_count = 0;
    bc->name = NULL;
    bc->source_file = NULL;
    return bc;
}

int bytecode_add_constant(Bytecode *bc, Value v) {
    bc->constants = (Value*)realloc(bc->constants, sizeof(Value) * (bc->const_count + 1));
    bc->constants[bc->const_count] = copy_value(&v);
    return bc->const_count++;
}

int bytecode_add_instruction(Bytecode *bc, OpCode op, int32_t operand) {
    bc->instructions = (Instruction*)realloc(bc->instructions, sizeof(Instruction) * (bc->instr_count + 1));
    bc->instructions[bc->instr_count].op = op;
    bc->instructions[bc->instr_count].operand = operand;
    return bc->instr_count++;
}

void bytecode_set_operand(Bytecode *bc, int idx, int32_t operand) {
    if (idx >= 0 && idx < bc->instr_count) {
        bc->instructions[idx].operand = operand;
    }
}

void bytecode_free(Bytecode *bc) {
    if (!bc) return;
    for (int i = 0; i < bc->const_count; ++i) {
        free_value(bc->constants[i]);
    }
    free(bc->constants);
    free(bc->instructions);
    if (bc->name) free((void*)bc->name);
    if (bc->source_file) free((void*)bc->source_file);
    free(bc);
}

static const char *opcode_name(OpCode op) {
    switch (op) {
        case OP_NOP: return "NOP";
        case OP_LOAD_CONST: return "LOAD_CONST";
        case OP_LOAD_LOCAL: return "LOAD_LOCAL";
        case OP_STORE_LOCAL: return "STORE_LOCAL";
        case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
        case OP_STORE_GLOBAL: return "STORE_GLOBAL";
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_LT: return "LT";
        case OP_LTE: return "LTE";
        case OP_GT: return "GT";
        case OP_GTE: return "GTE";
        case OP_EQ: return "EQ";
        case OP_NEQ: return "NEQ";
        case OP_POP: return "POP";
        case OP_JUMP: return "JUMP";
        case OP_JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OP_CALL: return "CALL";
        case OP_RETURN: return "RETURN";
        case OP_PRINT: return "PRINT";
        case OP_ECHO: return "ECHO";
        case OP_HALT: return "HALT";
        case OP_MOD: return "MOD";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_DUP: return "DUP";
        case OP_SWAP: return "SWAP";
        case OP_MAKE_ARRAY: return "MAKE_ARRAY";
        case OP_INDEX_GET: return "INDEX_GET";
        case OP_INDEX_SET: return "INDEX_SET";
        case OP_LEN: return "LEN";
        case OP_PUSH: return "ARR_PUSH";
        case OP_APOP: return "ARR_POP";
        case OP_SET: return "ARR_SET";
        case OP_INSERT: return "ARR_INSERT";
        case OP_REMOVE: return "ARR_REMOVE";
        case OP_SLICE: return "SLICE";
        case OP_TO_NUMBER: return "TO_NUMBER";
        case OP_TO_STRING: return "TO_STRING";
        case OP_TYPEOF: return "TYPEOF";
        case OP_CAST: return "CAST";
        case OP_SPLIT: return "SPLIT";
        case OP_JOIN: return "JOIN";
        case OP_SUBSTR: return "SUBSTR";
        case OP_FIND: return "FIND";
        case OP_REGEX_MATCH: return "REGEX_MATCH";
        case OP_REGEX_SEARCH: return "REGEX_SEARCH";
        case OP_REGEX_REPLACE: return "REGEX_REPLACE";
        case OP_CONTAINS: return "CONTAINS";
        case OP_INDEX_OF: return "INDEX_OF";
        case OP_CLEAR: return "CLEAR";
        case OP_ENUMERATE: return "ENUMERATE";
        case OP_ZIP: return "ZIP";
        case OP_MIN: return "MIN";
        case OP_MAX: return "MAX";
        case OP_CLAMP: return "CLAMP";
        case OP_ABS: return "ABS";
        case OP_POW: return "POW";
        case OP_RANDOM_SEED: return "RANDOM_SEED";
        case OP_RANDOM_INT: return "RANDOM_INT";
        case OP_MAKE_MAP: return "MAKE_MAP";
        case OP_KEYS: return "KEYS";
        case OP_VALUES: return "VALUES";
        case OP_HAS_KEY: return "HAS_KEY";
        case OP_READ_FILE: return "READ_FILE";
        case OP_WRITE_FILE: return "WRITE_FILE";
        case OP_ENV: return "ENV";
        case OP_INPUT_LINE: return "INPUT_LINE";
        case OP_PROC_RUN: return "PROC_RUN";
        case OP_PROC_SYSTEM: return "PROC_SYSTEM";
        case OP_TIME_NOW_MS: return "TIME_NOW_MS";
        case OP_CLOCK_MONO_MS: return "CLOCK_MONO_MS";
        case OP_DATE_FORMAT: return "DATE_FORMAT";
        case OP_ENV_ALL: return "ENV_ALL";
        case OP_FUN_VERSION: return "FUN_VERSION";
        case OP_THREAD_SPAWN: return "THREAD_SPAWN";
        case OP_THREAD_JOIN: return "THREAD_JOIN";
        case OP_SLEEP_MS: return "SLEEP_MS";
        case OP_RANDOM_NUMBER: return "RANDOM_NUMBER";
        case OP_BAND: return "BAND";
        case OP_BOR: return "BOR";
        case OP_BXOR: return "BXOR";
        case OP_BNOT: return "BNOT";
        case OP_SHL: return "SHL";
        case OP_SHR: return "SHR";
        case OP_ROTL: return "ROTL";
        case OP_ROTR: return "ROTR";
        case OP_JSON_PARSE: return "JSON_PARSE";
        case OP_JSON_STRINGIFY: return "JSON_STRINGIFY";
        case OP_JSON_FROM_FILE: return "JSON_FROM_FILE";
        case OP_JSON_TO_FILE: return "JSON_TO_FILE";
        case OP_CURL_GET: return "CURL_GET";
        case OP_CURL_POST: return "CURL_POST";
        case OP_CURL_DOWNLOAD: return "CURL_DOWNLOAD";
        case OP_SQLITE_OPEN: return "SQLITE_OPEN";
        case OP_SQLITE_CLOSE: return "SQLITE_CLOSE";
        case OP_SQLITE_EXEC: return "SQLITE_EXEC";
        case OP_SQLITE_QUERY: return "SQLITE_QUERY";
        case OP_LIBSQL_OPEN: return "LIBSQL_OPEN";
        case OP_LIBSQL_CLOSE: return "LIBSQL_CLOSE";
        case OP_LIBSQL_EXEC: return "LIBSQL_EXEC";
        case OP_LIBSQL_QUERY: return "LIBSQL_QUERY";
        case OP_PCSC_ESTABLISH: return "PCSC_ESTABLISH";
        case OP_PCSC_RELEASE: return "PCSC_RELEASE";
        case OP_PCSC_LIST_READERS: return "PCSC_LIST_READERS";
        case OP_PCSC_CONNECT: return "PCSC_CONNECT";
        case OP_PCSC_DISCONNECT: return "PCSC_DISCONNECT";
        case OP_PCSC_TRANSMIT: return "PCSC_TRANSMIT";
        case OP_PCRE2_TEST: return "PCRE2_TEST";
        case OP_PCRE2_MATCH: return "PCRE2_MATCH";
        case OP_PCRE2_FINDALL: return "PCRE2_FINDALL";
        case OP_INI_LOAD: return "INI_LOAD";
        case OP_INI_FREE: return "INI_FREE";
        case OP_INI_GET_STRING: return "INI_GET_STRING";
        case OP_INI_GET_INT: return "INI_GET_INT";
        case OP_INI_GET_DOUBLE: return "INI_GET_DOUBLE";
        case OP_INI_GET_BOOL: return "INI_GET_BOOL";
        case OP_INI_SET: return "INI_SET";
        case OP_INI_UNSET: return "INI_UNSET";
        case OP_INI_SAVE: return "INI_SAVE";
        case OP_XML_PARSE: return "XML_PARSE";
        case OP_XML_ROOT: return "XML_ROOT";
        case OP_XML_NAME: return "XML_NAME";
        case OP_XML_TEXT: return "XML_TEXT";
        case OP_TK_EVAL: return "TK_EVAL";
        case OP_TK_RESULT: return "TK_RESULT";
        case OP_TK_LOOP: return "TK_LOOP";
        case OP_TK_WM_TITLE: return "TK_WM_TITLE";
        case OP_TK_LABEL: return "TK_LABEL";
        case OP_TK_BUTTON: return "TK_BUTTON";
        case OP_TK_PACK: return "TK_PACK";
        case OP_FLOOR: return "FLOOR";
        case OP_CEIL: return "CEIL";
        case OP_TRUNC: return "TRUNC";
        case OP_ROUND: return "ROUND";
        case OP_SIN: return "SIN";
        case OP_COS: return "COS";
        case OP_TAN: return "TAN";
        case OP_EXP: return "EXP";
        case OP_LOG: return "LOG";
        case OP_LOG10: return "LOG10";
        case OP_SQRT: return "SQRT";
        case OP_GCD: return "GCD";
        case OP_LCM: return "LCM";
        case OP_ISQRT: return "ISQRT";
        case OP_SIGN: return "SIGN";
        case OP_FMIN: return "FMIN";
        case OP_FMAX: return "FMAX";
        case OP_RUST_HELLO: return "RUST_HELLO";
        case OP_RUST_HELLO_ARGS: return "RUST_HELLO_ARGS";
        default: return "???";
    }
}

void bytecode_dump(const Bytecode *bc) {
    if (!bc) {
        printf("<null bytecode>\n");
        return;
    }
    printf("Constants (%d):\n", bc->const_count);
    for (int i = 0; i < bc->const_count; ++i) {
        printf("  [%d] ", i);
        print_value(&bc->constants[i]);
        printf("\n");
    }
    printf("Instructions (%d):\n", bc->instr_count);
    for (int i = 0; i < bc->instr_count; ++i) {
        const Instruction *ins = &bc->instructions[i];
        printf("  %3d: %-15s %d\n", i, opcode_name(ins->op), ins->operand);
    }
}
