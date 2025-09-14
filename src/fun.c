#include "bytecode.h"
#include "value.h"
#include "vm.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef FUN_VERSION
#define FUN_VERSION "0.0.0-dev"
#endif

static int is_blank_line(const char *s) {
    for (const char *p = s; *p; ++p) {
        if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') return 0;
    }
    return 1;
}

static void print_usage(const char *prog) {
    printf("Fun %s\n", FUN_VERSION);
    printf("Usage:\n");
    printf("  %s [script.fun]\n", prog ? prog : "fun");
    printf("  %s --help | -h\n", prog ? prog : "fun");
    printf("  %s --version | -V\n", prog ? prog : "fun");
    printf("\n");
    printf("When no script is provided, a REPL starts. Submit an empty line to execute the buffer.\n");
}

int main(int argc, char **argv) {
    VM vm;
    vm_init(&vm);

    // CLI flags
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

    // If a script path is provided, parse and run it
    if (argc > 1) {
        const char *path = argv[1];
        Bytecode *bc = parse_file_to_bytecode(path);
        if (!bc) {
            fprintf(stderr, "Failed to compile script: %s\n", path);
            return 1;
        }
        vm_run(&vm, bc);

        // Print captured output for user scripts
        vm_print_output(&vm);
        vm_clear_output(&vm);

        bytecode_free(bc);
        return 0;
    }

    // REPL mode
    printf("Fun REPL. Type code and press Enter. Submit an empty line to run. Type 'exit' or 'quit' to leave.\n");
    char *buffer = NULL;
    size_t bufcap = 0;
    size_t buflen = 0;

    for (;;) {
        fputs(buflen == 0 ? "fun> " : "... ", stdout);
        fflush(stdout);

        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) {
            puts("");
            break; // EOF
        }

        // Exit commands
        if (buflen == 0 && (strcmp(line, "exit\n") == 0 || strcmp(line, "quit\n") == 0)) {
            break;
        }

        // Empty line -> compile and run accumulated buffer
        if (is_blank_line(line)) {
            if (buflen == 0) continue; // ignore extra empty lines
            // Null-terminate
            if (buflen + 1 >= bufcap) {
                bufcap = buflen + 1;
                buffer = (char*)realloc(buffer, bufcap);
            }
            buffer[buflen] = '\0';

            Bytecode *bc = parse_string_to_bytecode(buffer);
            if (bc) {
                vm_run(&vm, bc);
                vm_print_output(&vm);
                vm_clear_output(&vm);
                bytecode_free(bc);
            }
            // reset buffer
            buflen = 0;
            continue;
        }

        // Append line to buffer
        size_t linelen = strlen(line);
        if (buflen + linelen + 1 > bufcap) {
            size_t newcap = bufcap == 0 ? 1024 : bufcap * 2;
            while (newcap < buflen + linelen + 1) newcap *= 2;
            buffer = (char*)realloc(buffer, newcap);
            bufcap = newcap;
        }
        memcpy(buffer + buflen, line, linelen);
        buflen += linelen;
    }

    free(buffer);
    return 0;
}

