/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file try_pop.c
 * @brief Implements the OP_TRY_POP opcode to end a try/catch region.
 *
 * This file handles the OP_TRY_POP instruction, which marks the end of the
 * most recently started try/catch region in the current frame by popping its
 * entry from the per-frame TRY stack.
 *
 * Behavior:
 * - If f->try_sp >= 0, decrements f->try_sp (pops one TRY region).
 * - Does not modify the value stack.
 *
 * Errors:
 * - None; popping when no TRY is active is a no-op.
 */

case OP_TRY_POP: {
  if (f->try_sp >= 0) f->try_sp--;
  break;
}
