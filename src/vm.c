#define _GNU_SOURCE
#include "vm.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

void vm_clear_output(VM *vm) {
    for (int i = 0; i < vm->output_count; ++i) {
        free_value(vm->output[i]);
    }
    vm->output_count = 0;
}

void vm_free(VM *vm) {
    //if (vm->output) free(vm->output);
    //vm->output = NULL;
    //vm->output_count = 0;
}

static void push_value(VM *vm, Value v) {
    if (vm->sp >= VM_STACK_SIZE - 1) {
        fprintf(stderr, "Runtime error: stack overflow\n");
        exit(1);
    }
    vm->stack[++vm->sp] = v; /* take ownership of v */
}

static Value pop_value(VM *vm) {
    if (vm->sp < 0) {
        fprintf(stderr, "Runtime error: stack underflow\n");
        exit(1);
    }
    return vm->stack[vm->sp--]; /* caller owns returned Value */
}

static void frame_init(Frame *f) {
    f->fn = NULL;
    f->ip = 0;
    for (int i = 0; i < FRAME_MAX_LOCALS; ++i) f->locals[i] = make_nil();
}

void vm_init(VM *vm) {
    vm->sp = -1;
    vm->fp = -1;
    vm->output_count = 0;
    for (int i = 0; i < VM_MAX_GLOBALS; ++i)
        vm->globals[i] = make_nil();
}

/* push a new frame, transferring ownership of args[] into frame->locals[0..argc-1] */
static void vm_push_frame(VM *vm, Bytecode *fn, int argc, Value *args) {
    if (vm->fp >= VM_MAX_FRAMES - 1) {
        fprintf(stderr, "Runtime error: too many frames\n");
        exit(1);
    }
    Frame *f = &vm->frames[++vm->fp];
    frame_init(f);
    f->fn = fn;
    f->ip = 0;
    /* move args into locals 0..argc-1 */
    for (int i = 0; i < argc && i < FRAME_MAX_LOCALS; ++i) {
        f->locals[i] = args[i]; /* transfer ownership */
    }
}

/* pop current frame and free its locals */
static void vm_pop_frame(VM *vm) {
    if (vm->fp < 0) {
        fprintf(stderr, "Runtime error: pop frame with empty frame stack\n");
        exit(1);
    }
    Frame *f = &vm->frames[vm->fp];
    for (int i = 0; i < FRAME_MAX_LOCALS; ++i) {
        free_value(f->locals[i]);
        f->locals[i] = make_nil();
    }
    vm->fp--;
}

void vm_print_output(VM *vm) {
    for (int i = 0; i < vm->output_count; ++i) {
        print_value(&vm->output[i]);
        printf("\n");
    }
}

void vm_run(VM *vm, Bytecode *entry) {
    /* start with entry frame (no args) */
    vm_push_frame(vm, entry, 0, NULL);

    while (vm->fp >= 0) {
        Frame *f = &vm->frames[vm->fp];

        if (f->ip < 0 || f->ip >= f->fn->instr_count) {
            /* no more instructions in this frame -> implicit return nil */
            Value nilv = make_nil();
            vm_pop_frame(vm);
            push_value(vm, nilv);
            continue;
        }

        Instruction inst = f->fn->instructions[f->ip++];
        switch (inst.op) {
            case OP_NOP:
                break;

            case OP_LOAD_CONST: {
                int idx = inst.operand;
                if (idx < 0 || idx >= f->fn->const_count) {
                    fprintf(stderr, "Runtime error: constant index out of range\n");
                    exit(1);
                }
                Value c = copy_value(&f->fn->constants[idx]);
                push_value(vm, c);
                break;
            }

            case OP_LOAD_LOCAL: {
                int slot = inst.operand;
                if (slot < 0 || slot >= FRAME_MAX_LOCALS) {
                    fprintf(stderr, "Runtime error: local slot out of range\n");
                    exit(1);
                }
                Value val = copy_value(&f->locals[slot]);
                push_value(vm, val);
                break;
            }

            case OP_STORE_LOCAL: {
                int slot = inst.operand;
                if (slot < 0 || slot >= FRAME_MAX_LOCALS) {
                    fprintf(stderr, "Runtime error: local slot out of range\n");
                    exit(1);
                }
                Value v = pop_value(vm);
                /* free previous local then move v into it */
                free_value(f->locals[slot]);
                f->locals[slot] = v;
                break;
            }

            case OP_ADD: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: ADD expects ints\n");
                    exit(1);
                }
                Value res = make_int(a.i + b.i);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_SUB: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: SUB expects ints\n");
                    exit(1);
                }
                Value res = make_int(a.i - b.i);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_MUL: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: MUL expects ints\n");
                    exit(1);
                }
                Value res = make_int(a.i * b.i);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_DIV: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: DIV expects ints\n");
                    exit(1);
                }
                if (b.i == 0) {
                    fprintf(stderr, "Runtime error: division by zero\n");
                    exit(1);
                }
                Value res = make_int(a.i / b.i);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_LT: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: LT expects ints\n");
                    exit(1);
                }
                Value res = make_int(a.i < b.i ? 1 : 0);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_LTE: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: LTE expects ints\n");
                    exit(1);
                }
                Value res = make_int(a.i <= b.i ? 1 : 0);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_JUMP: {
                f->ip = inst.operand;
                break;
            }

            case OP_GT: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: GT expects ints\n");
                    exit(1);
                }
                push_value(vm, make_int(a.i > b.i ? 1 : 0));
                free_value(a); free_value(b);
                break;
            }

            case OP_GTE: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: GTE expects ints\n");
                    exit(1);
                }
                push_value(vm, make_int(a.i >= b.i ? 1 : 0));
                free_value(a); free_value(b);
                break;
            }

            case OP_EQ: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                push_value(vm, make_int((a.type == b.type && a.i == b.i) ? 1 : 0)); // for ints only, extend later
                free_value(a); free_value(b);
                break;
            }

            case OP_NEQ: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                push_value(vm, make_int((a.type != b.type || a.i != b.i) ? 1 : 0)); // for ints only
                free_value(a); free_value(b);
                break;
            }

            case OP_JUMP_IF_FALSE: {
                Value cond = pop_value(vm);
                int truthy = value_is_truthy(&cond);
                free_value(cond);
                if (!truthy) {
                    f->ip = inst.operand;
                }
                break;
            }

            case OP_CALL: {
                int argc = inst.operand;
                if (argc < 0) argc = 0;
                /* collect args in reverse (preserve order) */
                Value *args = NULL;
                if (argc > 0) {
                    args = (Value*)malloc(sizeof(Value) * argc);
                    /* pop args into array in reverse */
                    for (int i = argc - 1; i >= 0; --i) {
                        args[i] = pop_value(vm);
                    }
                }
                /* pop function value */
                Value fnv = pop_value(vm);
                if (fnv.type != VAL_FUNCTION) {
                    fprintf(stderr, "Runtime type error: CALL expects function\n");
                    exit(1);
                }
                /* push new frame and transfer args */
                vm_push_frame(vm, fnv.fn, argc, args);
                /* free args array (locals moved), free fnv (no-op for function) */
                free(args);
                /* note: fnv contains a pointer to the Bytecode, don't free here */
                break;
            }

            case OP_RETURN: {
                Value retv;
                /* if there's something on the stack -> return it, else nil */
                if (vm->sp >= 0) retv = pop_value(vm);
                else retv = make_nil();

                /* pop frame (free locals) */
                vm_pop_frame(vm);

                /* push return value into caller frame (or onto stack if no caller) */
                push_value(vm, retv);
                break;
            }

            case OP_PRINT: {
                Value v = pop_value(vm);
                if (vm->output_count < VM_OUTPUT_SIZE) {
                    vm->output[vm->output_count++] = v;  // store instead of printing
                } else {
                    free_value(v);  // prevent leak
                    fprintf(stderr, "Runtime error: output buffer overflow\n");
                    exit(1);
                }
                break;
            }

            case OP_HALT:
                return;

            case OP_POP: {
                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: stack underflow for POP\n");
                    exit(1);
                }
                Value v = pop_value(vm);
                free_value(v); // free the popped value
                break;
            }

            case OP_DUP: {
                if (vm->sp < 0) {
                    fprintf(stderr, "Runtime error: stack underflow for DUP\n");
                    exit(1);
                }
                Value top = vm->stack[vm->sp];
                push_value(vm, copy_value(&top));
                break;
            }

            case OP_SWAP: {
                if (vm->sp < 1) {
                    fprintf(stderr, "Runtime error: stack underflow for SWAP\n");
                    exit(1);
                }
                Value a = vm->stack[vm->sp];
                Value b = vm->stack[vm->sp - 1];
                vm->stack[vm->sp] = b;
                vm->stack[vm->sp - 1] = a;
                break;
            }

            case OP_LOAD_GLOBAL: {
                int idx = inst.operand;
                if (idx < 0 || idx >= VM_MAX_GLOBALS) {
                    fprintf(stderr, "Runtime error: global index out of range\n");
                    exit(1);
                }
                push_value(vm, copy_value(&vm->globals[idx]));
                break;
            }

            case OP_STORE_GLOBAL: {
                int idx = inst.operand;
                if (idx < 0 || idx >= VM_MAX_GLOBALS) {
                    fprintf(stderr, "Runtime error: global index out of range\n");
                    exit(1);
                }
                Value v = pop_value(vm);
                free_value(vm->globals[idx]);
                vm->globals[idx] = v;
                break;
            }

            case OP_MOD: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                if (a.type != VAL_INT || b.type != VAL_INT) {
                    fprintf(stderr, "Runtime type error: MOD expects ints\n");
                    exit(1);
                }
                if (b.i == 0) {
                    fprintf(stderr, "Runtime error: modulo by zero\n");
                    exit(1);
                }
                Value res = make_int(a.i % b.i);
                free_value(a);
                free_value(b);
                push_value(vm, res);
                break;
            }

            case OP_AND: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                int res = value_is_truthy(&a) && value_is_truthy(&b);
                free_value(a);
                free_value(b);
                push_value(vm, make_int(res));
                break;
            }

            case OP_OR: {
                Value b = pop_value(vm);
                Value a = pop_value(vm);
                int res = value_is_truthy(&a) || value_is_truthy(&b);
                free_value(a);
                free_value(b);
                push_value(vm, make_int(res));
                break;
            }

            case OP_NOT: {
                Value v = pop_value(vm);
                int res = !value_is_truthy(&v);
                free_value(v);
                push_value(vm, make_int(res));
                break;
            }

            default:
                if (!opcode_is_valid(inst.op)) {
                    fprintf(stderr, "Runtime error: unknown opcode %d (%s) at instruction %d\n",
                            inst.op,
                            (inst.op >= 0 && inst.op < sizeof(opcode_names)/sizeof(opcode_names[0]))
                                ? opcode_names[inst.op] : "???",
                            f->ip - 1);
                    exit(1);
                }
                break;
        }
    }
}

