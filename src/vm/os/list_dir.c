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

case OP_OS_LIST_DIR: {
    /* pops path string; pushes array of strings */
    Value pathv = pop_value(vm);
    char *path = value_to_string_alloc(&pathv);
    free_value(pathv);

    Value arr = make_array_from_values(NULL, 0);
    if (path) {
        /* 
         * Using 'ls -1' as a fallback to avoid dirent.h conflicts on some systems.
         * We escape the path minimally for the shell. 
         */
        size_t slen = strlen(path) + 16;
        char *cmd = (char*)malloc(slen);
        if (cmd) {
            snprintf(cmd, slen, "ls -1 \"%s\"", path);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                char line[1024];
                while (fgets(line, sizeof(line), fp)) {
                    /* Strip trailing newline */
                    size_t l = strlen(line);
                    if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
                    if (l > 1 && line[l-2] == '\r') line[l-2] = '\0';
                    
                    if (line[0] != '\0') {
                        array_push(&arr, make_string(line));
                    }
                }
                pclose(fp);
            }
            free(cmd);
        }
        free(path);
    }
    push_value(vm, arr);
    break;
}
