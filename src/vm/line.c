/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file line.c
 * @brief Implements the OP_LINE pseudo-opcode to update the current source line.
 *
 * This snippet is included into the VM dispatch loop and handles the OP_LINE
 * instruction. It records the source line number carried in the instruction's
 * operand so that runtime errors and debugger output can reference the correct
 * line in the original program.
 *
 * Stack contract: none (does not read or write the VM value stack).
 */

case OP_LINE: {
  /* operand holds the source line number */
  vm->current_line = inst.operand;
  break;
}
