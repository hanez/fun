/**
 * Main entry point for the Fun language interpreter.
 * Builds a CLI that runs a script file if provided; otherwise starts the REPL
 * when compiled with FUN_WITH_REPL enabled.
 */

#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    printf("  %s [script.fun]\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("When no script is provided, a REPL starts. Submit an empty line to execute the buffer.\n");
#else
    printf("  %s <script.fun>\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("REPL is disabled in this build. Please provide a script file to run.\n");
#endif
}

int main(int argc, char **argv) {
    VM vm;
    vm_init(&vm);

    if (argc > 1) {
        const char *arg = argv[1];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(arg, "--version") == 0 || strcmp(arg, "-V") == 0) {
            printf("Fun %s\n", FUN_VERSION);
            return 0;
        }
    }

#ifndef FUN_WITH_REPL
    if (argc <= 1) {
        fprintf(stderr, "Error: REPL is disabled. Please provide a script to run.\n");
        print_usage(argv[0]);
        return 2;
    }
#endif

    if (argc > 1) {
        const char *path = argv[1];
        Bytecode *bc = parse_file_to_bytecode(path);
        if (!bc) {
            fprintf(stderr, "Failed to compile script: %s\n", path);
            return 1;
        }

        vm_run(&vm, bc);
        vm_print_output(&vm);
        vm_clear_output(&vm);
        bytecode_free(bc);
        return 0;
    }

#ifdef FUN_WITH_REPL
    return fun_run_repl(&vm);
#else
    fprintf(stderr, "Internal error: REPL not available in this build.\n");
    return 2;
#endif
}
