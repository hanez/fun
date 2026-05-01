/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file free.c
 * @brief VM opcode snippet for releasing an INI handle (OP_INI_FREE).
 *
 * This file is included into the main VM dispatch switch in vm.c. It is only
 * compiled when FUN_WITH_INI is enabled and iniparser headers are available.
 *
 * Opcode: OP_INI_FREE
 * Stack: [handle:int] -> [ok:int]
 *
 * Behavior
 * - Pops an integer handle referring to an INI dictionary previously returned
 *   by OP_INI_LOAD.
 * - Attempts to close the underlying dictionary and free the registry slot.
 * - Pushes 1 on success, 0 if the handle was invalid or already freed.
 *
 * Errors
 * - No VM error is thrown for invalid handles; the opcode simply returns 0.
 *
 * See also
 * - ini_alloc_handle(), ini_free_handle() in src/vm/ini/handles.c
 */
/* OP_INI_FREE: pops handle; pushes 1/0 */
#ifdef FUN_WITH_INI
case OP_INI_FREE: {
  Value vh = pop_value(vm);
  int h = (vh.type == VAL_INT) ? (int)vh.i : 0;
  free_value(vh);
  int ok = ini_free_handle(h);
  push_value(vm, make_int(ok));
  break;
}
#endif
