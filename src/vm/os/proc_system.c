/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-02
 */

#ifdef __unix__
#include <sys/wait.h>
#endif

case OP_PROC_SYSTEM: {
    /* Pops command string; pushes exit code number */
    Value cmdv = pop_value(vm);
    char *cmd = value_to_string_alloc(&cmdv);
    free_value(cmdv);
    if (!cmd) {
        push_value(vm, make_int(-1));
        break;
    }
    int status = system(cmd);
    int code = -1;
#ifdef __unix__
    if (status == -1) code = -1;
    else if (WIFEXITED(status)) code = WEXITSTATUS(status);
    else code = -1;
#else
    code = status;
#endif
    push_value(vm, make_int(code));
    free(cmd);
    break;
}
