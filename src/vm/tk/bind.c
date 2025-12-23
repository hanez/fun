/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-12-23
 */

/* TK_BIND */
case OP_TK_BIND: {
    /* stack: ..., id, event, command -> rc */
    Value cmdv = pop_value(vm);
    Value eventv = pop_value(vm);
    Value idv = pop_value(vm);
    char *cmd = value_to_string_alloc(&cmdv);
    char *event = value_to_string_alloc(&eventv);
    char *id = value_to_string_alloc(&idv);
    free_value(cmdv);
    free_value(eventv);
    free_value(idv);

    if (!id || !event || !cmd) {
        if (id) free(id);
        if (event) free(event);
        if (cmd) free(cmd);
        push_value(vm, make_int(-1));
        break;
    }

    /* 
     * Construct: bind .id <event> {command}
     * Note: for now, command is just raw Tcl as well, 
     * but could be extended to call Fun functions if we had a callback mechanism.
     */
    size_t slen = strlen(id) + strlen(event) + strlen(cmd) + 32;
    char *script = (char*)malloc(slen);
    if (!script) {
        free(id); free(event); free(cmd);
        push_value(vm, make_int(-1));
        break;
    }
    
    snprintf(script, slen, "bind .%s %s {%s}", id, event, cmd);
    int rc = fun_tk_eval_script(script);
    free(script);
    free(id);
    free(event);
    free(cmd);
    push_value(vm, make_int(rc));
    break;
}
