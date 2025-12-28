/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 * 
 * Added: 2025-12-28
 */

// Get all environment variables of the operation system and push them as a map.

case OP_ENV_ALL: {
    extern char **environ;
    Value m = make_map_empty();
    if (environ) {
        for (char **env = environ; *env; ++env) {
            char *entry = *env;
            char *equals = strchr(entry, '=');
            if (equals) {
                size_t key_len = equals - entry;
                char *key = malloc(key_len + 1);
                if (key) {
                    memcpy(key, entry, key_len);
                    key[key_len] = '\0';
                    map_set(&m, key, make_string(equals + 1));
                    free(key);
                }
            }
        }
    }
    push_value(vm, m);
    break;
}
