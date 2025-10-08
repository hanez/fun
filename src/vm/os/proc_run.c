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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #define popen  _popen
  #define pclose _pclose
#endif

case OP_PROC_RUN: {
    /* Pops command string; pushes map {"out": string, "code": int} */
    Value cmdv = pop_value(vm);
    char *cmd = value_to_string_alloc(&cmdv);
    free_value(cmdv);
    if (!cmd) {
        Value m = make_map_empty();
        map_set(&m, "out", make_string(""));
        map_set(&m, "code", make_int(-1));
        push_value(vm, m);
        break;
    }

    /* Use popen to run via shell and capture stdout */
    FILE *fp = popen(cmd, "r");
    int exit_code = -1;
    char *out = NULL;
    size_t cap = 0, len = 0;
    if (fp) {
        cap = 4096;
        out = (char*)malloc(cap);
        if (!out) {
            pclose(fp);
            free(cmd);
            Value m = make_map_empty();
            map_set(&m, "out", make_string(""));
            map_set(&m, "code", make_int(-1));
            push_value(vm, m);
            break;
        }
        int c;
        while ((c = fgetc(fp)) != EOF) {
            if (len + 1 >= cap) {
                cap *= 2;
                char *nb = (char*)realloc(out, cap);
                if (!nb) {
                    free(out);
                    pclose(fp);
                    free(cmd);
                    Value m = make_map_empty();
                    map_set(&m, "out", make_string(""));
                    map_set(&m, "code", make_int(-1));
                    push_value(vm, m);
                    goto done_push;
                }
                out = nb;
            }
            out[len++] = (char)c;
        }
        out[len] = '\0';
        int status = pclose(fp);
#ifdef __unix__
        if (WIFEXITED(status)) exit_code = WEXITSTATUS(status);
        else exit_code = -1;
#else
        exit_code = status;
#endif
    } else {
        out = strdup("");
        exit_code = -1;
    }

    Value m = make_map_empty();
    map_set(&m, "out", make_string(out ? out : ""));
    map_set(&m, "code", make_int(exit_code));
    push_value(vm, m);

done_push:
    if (out) free(out);
    free(cmd);
    break;
}
