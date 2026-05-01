/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file thread_spawn.c
 * @brief Implements OP_THREAD_SPAWN to run a function in a background thread.
 *
 * Behavior:
 * - Pops function and optionally one argument or an array of arguments (controlled by operand).
 * - Spawns a new thread that invokes the function; pushes a thread id (int) or 0 on failure.
 *
 * Errors:
 * - If types are wrong or spawning fails, returns 0.
 */

case OP_THREAD_SPAWN: {
  /* operand: 0 -> no args; 1 -> has args array or single arg */
  Value argsMaybe = make_nil();
  if (inst.operand == 1) {
    argsMaybe = pop_value(vm); /* maybe array or scalar */
  }
  Value fnv = pop_value(vm);
  int tid = fun_thread_spawn(fnv, argsMaybe, inst.operand == 1);
  free_value(fnv);
  if (inst.operand == 1) free_value(argsMaybe);
  push_value(vm, make_int(tid));
  break;
}
