/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
* @file halt.c
 * @brief Implements the OP_HALT opcode for stopping VM execution.
 *
 * This file handles the OP_HALT instruction, which stops the execution of the VM.
 * No stack operations are performed.
 *
 * Behavior:
 * - Stops the VM execution immediately.
 *
 * Example:
 * // Bytecode: OP_HALT
 * // Stack before: [42]
 * // Stack after: [42]
 *
 * @author Johannes Findeisen
 * @date 2025-10-16
 */

case OP_HALT:
    return;
