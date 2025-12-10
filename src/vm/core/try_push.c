/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_TRY_PUSH: {
    /* push index of this TRY instruction; handler ip is in its operand (may be patched later) */
    if (f->try_sp >= (int)(sizeof(f->try_stack)/sizeof(f->try_stack[0])) - 1) {
        fprintf(stderr, "Runtime error: try depth exceeded\n");
        exit(1);
    }
    f->try_stack[++f->try_sp] = f->ip - 1; /* index of TRY_PUSH instruction */
    break;
}
