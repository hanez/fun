/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file jump_if_false.c
 * @brief Implements the OP_JUMP_IF_FALSE opcode for conditional jumps in the VM.
 *
 * This file handles the OP_JUMP_IF_FALSE instruction, which jumps if the top
 * stack value is falsey (0, false, nil, etc).
 *
 * Behavior:
 * - Pops condition value from stack
 * - Jumps to operand IP if falsey
 * - Continues normally if truthy
 *
 * Used for:
 * - If statements
 * - While loops
 * - Logical expressions
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_JUMP_IF_FALSE: {
    Value cond = pop_value(vm);
    int truthy = value_is_truthy(&cond);
    free_value(cond);
    if (!truthy) {
        f->ip = inst.operand;
    }
    break;
}
