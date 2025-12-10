/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

case OP_THROW: {
    Value err = pop_value(vm);
    /* if there is a handler in this frame, jump to it and push err for catch */
    if (f->try_sp >= 0) {
        int try_idx = f->try_stack[f->try_sp--];
        int target = f->fn->instructions[try_idx].operand;
        /* push error for catch block */
        push_value(vm, err); /* transfer ownership to stack */
        f->ip = target;
        break;
    }
    /* Unhandled: print error and terminate */
    char *s = value_to_string_alloc(&err);
    if (s) {
        fprintf(stdout, "%s\n", s);
        free(s);
    } else {
        fprintf(stdout, "<error>\n");
    }
    free_value(err);
    /* clear frames to stop execution */
    vm->fp = -1;
    break;
}
