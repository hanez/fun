#include "vm.h"

const char *opcode_names[] = {
    "OP_NOP", "OP_LOAD_CONST", "OP_LOAD_LOCAL", "OP_STORE_LOCAL",
    "OP_LOAD_GLOBAL", "OP_STORE_GLOBAL", "OP_ADD", "OP_SUB",
    "OP_MUL", "OP_DIV", "OP_LT", "OP_LTE",
    "OP_GT", "OP_GTE", "OP_EQ", "OP_NEQ",
    "OP_POP", "OP_JUMP", "OP_JUMP_IF_FALSE", "OP_CALL",
    "OP_RETURN", "OP_PRINT", "OP_MOD", "OP_AND",
    "OP_OR", "OP_NOT", "OP_DUP", "OP_SWAP"
};

