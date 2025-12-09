/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-09
 */

/* TK_LOOP */
case OP_TK_LOOP: {
    fun_tk_loop();
#ifdef FUN_WITH_TCLTK
    /* Ensure the process terminates once the GUI window(s) are closed. */
    exit(0);
#else
    /* When Tk support is not compiled in, behave as a no-op returning Nil. */
    push_value(vm, make_nil());
#endif
    break; /* not reached when FUN_WITH_TCLTK */
}
