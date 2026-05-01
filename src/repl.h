/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */
 
/**
 * @file repl.h
 * @brief Interactive Read-Eval-Print Loop (REPL) entry point.
 */
#ifndef FUN_REPL_H
#define FUN_REPL_H

#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FUN_WITH_REPL
/**
 * @brief Run the interactive REPL using an already-initialized VM.
 *
 * Reads lines from stdin, evaluates them in the provided VM context, and
 * prints results/errors. The function returns on EOF or when the user issues
 * a quit command supported by the REPL implementation.
 *
 * @param vm Pointer to an initialized VM; must not be NULL.
 * @return 0 on normal exit, non-zero on fatal error.
 */
int fun_run_repl(VM *vm);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FUN_REPL_H
