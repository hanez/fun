/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file load_const.c
 * @brief Implements the OP_LOAD_CONST opcode for loading constants in the VM.
 *
 * This file handles the OP_LOAD_CONST instruction, which loads a constant value
 * from the bytecode's constant pool onto the stack.
 *
 * Behavior:
 * - operand is index into constant pool
 * - Pushes copy of constant onto stack
 *
 * Error Handling:
 * - Exits if invalid constant index
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

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
