/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file jump.c
 * @brief Implements the OP_JUMP opcode for unconditional jumps in the VM.
 *
 * This file handles the OP_JUMP instruction, which performs an unconditional
 * jump to a new instruction pointer location.
 *
 * Behavior:
 * - Sets IP to operand value
 * - No stack manipulation
 *
 * Used for:
 * - Loops
 * - Function returns
 * - Conditional control flow
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_JUMP: {
    f->ip = inst.operand;
    break;
}
