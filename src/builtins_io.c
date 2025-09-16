/**
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File I/O helpers: return malloc'd buffers or status ints */

char *bio_read_file_strdup(const char *path) {
    if (!path) return strdup("");
    FILE *f = fopen(path, "rb");
    if (!f) return strdup("");
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return strdup(""); }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return strdup(""); }
    rewind(f);
    char *buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return strdup(""); }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

int bio_write_file(const char *path, const char *data, size_t len) {
    if (!path) return 0;
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    if (!data) data = "";
    int ok = (fwrite(data, 1, len, f) == len);
    fclose(f);
    return ok;
}
