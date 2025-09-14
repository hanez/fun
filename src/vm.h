#ifndef FUN_VM_H
#define FUN_VM_H

#include "bytecode.h"

#define VM_STACK_SIZE 1024
#define VM_MAX_FRAMES 128
#define VM_MAX_GLOBALS 128
#define VM_OUTPUT_SIZE 1024
#define FRAME_MAX_LOCALS 64

static const char *opcode_names[] = {
    "NOP","LOAD_CONST","LOAD_LOCAL","STORE_LOCAL",
    "LOAD_GLOBAL","STORE_GLOBAL","ADD","SUB","MUL","DIV",
    "LT","LTE","GT","GTE","EQ","NEQ","POP","JUMP",
    "JUMP_IF_FALSE","CALL","RETURN","PRINT","HALT",
    "MOD","AND","OR","NOT","DUP","SWAP",
    "MAKE_ARRAY","INDEX_GET","INDEX_SET",
    "LEN","ARR_PUSH","ARR_POP","ARR_SET","ARR_INSERT","ARR_REMOVE","SLICE"
};

typedef struct {
    Bytecode *fn;
    int ip;
    Value locals[FRAME_MAX_LOCALS];
} Frame;

typedef struct {
    Value stack[VM_STACK_SIZE];
    int sp;

    Frame frames[VM_MAX_FRAMES];
    int fp; // frame pointer, -1 when no frame

    Value globals[VM_MAX_GLOBALS];

    Value output[VM_OUTPUT_SIZE]; // store printed values
    int output_count;

    long long instr_count; // executed instructions in the last vm_run
} VM;

// initialize VM (zero state)
void vm_init(VM *vm);

// helper function to clear the output
void vm_clear_output(VM *vm);
void vm_print_output(VM *vm);
void vm_free(VM *vm);

// reset VM to initial state (free globals/locals/output; keep VM object)
void vm_reset(VM *vm);

// print non-nil globals (index and value) to stdout
void vm_dump_globals(VM *vm);
// run entry Bytecode (pushes first frame)
void vm_run(VM *vm, Bytecode *entry);

static inline int opcode_is_valid(int op) {
    return op >= OP_NOP && op <= OP_SLICE;  // all current opcodes
}

#endif

