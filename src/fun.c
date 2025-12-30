/*
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

 /*
 * Main entry point for the Fun language interpreter.
 * Builds a CLI that runs a script file if provided; otherwise starts the REPL
 * when compiled with FUN_WITH_REPL enabled.
 */

#include "bytecode.h"
#include "vm.h"
#include "parser.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef FUN_WITH_REPL
#include "repl.h"
#endif

#ifndef FUN_VERSION
#define FUN_VERSION "0.0.0-dev"
#endif

static void print_usage(const char *prog) {
    printf("Fun %s\n", FUN_VERSION);
    printf("Usage:\n");
#ifdef FUN_WITH_REPL
    printf("  %s [--trace|-t] [--repl-on-error] [script.fun]\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("Options:\n");
    printf("  --trace, -t       Print executed ops and stack tops during run\n");
    printf("  --repl-on-error   Enter interactive REPL on runtime error with stack preserved\n\n");
    printf("When no script is provided, a REPL starts. Submit an empty line to execute the buffer.\n");
#else
    printf("  %s [--trace|-t] <script.fun>\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("Options:\n  --trace, -t   Print executed ops and stack tops during run\n\n");
    printf("REPL is disabled in this build. Please provide a script file to run.\n");
#endif
}

int main(int argc, char **argv) {
    /* Set FUN_EXECUTABLE environment variable to the path of this binary */
    setenv("FUN_EXECUTABLE", argv[0], 1);

    VM vm;
    vm_init(&vm);

    int argi = 1;
    for (; argi < argc; ++argi) {
        const char *arg = argv[argi];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(arg, "--version") == 0 || strcmp(arg, "-V") == 0) {
            printf("Fun %s\n", FUN_VERSION);
            return 0;
        }
        if (strcmp(arg, "--trace") == 0 || strcmp(arg, "-t") == 0) {
            vm.trace_enabled = 1;
            continue;
        }
#ifdef FUN_WITH_REPL
        if (strcmp(arg, "--repl-on-error") == 0) {
            vm.repl_on_error = 1;
            vm.on_error_repl = fun_run_repl; /* provide REPL entry to core VM */
            continue;
        }
#endif
        /* first non-option assumed to be script path */
        break;
    }

#ifndef FUN_WITH_REPL
    if (argi >= argc) {
        fprintf(stderr, "Error: REPL is disabled. Please provide a script to run.\n");
        print_usage(argv[0]);
        return 2;
    }
#endif

    if (argi < argc) {
        const char *path = argv[argi];

        /* Collect script arguments (everything after script path) and expose via env vars */
        int sargi = argi + 1; /* first script arg following the script path */
        int sargc = (sargi < argc) ? (argc - sargi) : 0;

        /* Export FUN_ARGC */
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", sargc);
            setenv("FUN_ARGC", buf, 1);
        }

        /* Export FUN_ARGV_i */
        for (int i = 0; i < sargc; ++i) {
            char key[32];
            snprintf(key, sizeof(key), "FUN_ARGV_%d", i);
            setenv(key, argv[sargi + i], 1);
        }

        /* Optional: space-joined convenience string FUN_ARGS */
        if (sargc > 0) {
            size_t total = 0;
            for (int i = 0; i < sargc; ++i) {
                total += strlen(argv[sargi + i]) + 1; /* +1 for space or NUL */
            }
            char *joined = (char*)malloc(total);
            if (joined) {
                joined[0] = '\0';
                for (int i = 0; i < sargc; ++i) {
                    strcat(joined, argv[sargi + i]);
                    if (i + 1 < sargc) strcat(joined, " ");
                }
                setenv("FUN_ARGS", joined, 1);
                free(joined);
            }
        } else {
            /* Ensure FUN_ARGS is at least cleared for consistency */
            setenv("FUN_ARGS", "", 1);
        }

        Bytecode *bc = parse_file_to_bytecode(path);
        if (!bc) {
            fprintf(stderr, "Failed to compile script: %s\n", path);
            return 1;
        }

        vm_run(&vm, bc);
        vm_print_output(&vm);
        vm_clear_output(&vm);
        bytecode_free(bc);
        return vm.exit_code;
    }

#ifdef FUN_WITH_REPL
    return fun_run_repl(&vm);
#else
    fprintf(stderr, "Internal error: REPL not available in this build.\n");
    return 2;
#endif
}
