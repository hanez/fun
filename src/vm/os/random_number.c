/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 * 
 * Added: 2025-12-12
 */

/* Generate OS-based random bytes and return them hex-encoded.
 * Opcode: OP_RANDOM_NUMBER
 * Stack:  pops len (number of raw bytes), pushes hex string of length 2*len
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-specific headers guarded per OS to avoid leaking problematic macros */
#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  #include <bcrypt.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
  #include <stdlib.h>
  #include <sys/random.h>
#elif defined(__unix__)
  #if __has_include(<sys/random.h>)
    #include <sys/random.h>
  #endif
  #include <fcntl.h>
  #include <unistd.h>
#endif

case OP_RANDOM_NUMBER: {
    /* pop requested raw byte length */
    Value lv = pop_value(vm);
    if (lv.type != VAL_INT) {
        fprintf(stderr, "Runtime type error: random_number(len) expects integer length\n");
        free_value(lv);
        /* For safety, push empty string so callers expecting a value won't underflow */
        push_value(vm, make_string(""));
        break;
    }
    int64_t len = lv.i;
    if (len < 0) {
        fprintf(stderr, "random_number error: negative length (%" PRId64 ")\n", len);
        free_value(lv);
        push_value(vm, make_string(""));
        break;
    }
    if (len == 0) {
        free_value(lv);
        push_value(vm, make_string(""));
        break;
    }

    /* Cap to prevent excessive allocations (max 1 MiB raw -> 2 MiB hex) */
    const int64_t MAX_RAW = (1LL << 20);
    if (len > MAX_RAW) {
        fprintf(stderr, "random_number error: requested length too large (%" PRId64 ", max %" PRId64 ")\n", len, MAX_RAW);
        free_value(lv);
        push_value(vm, make_string(""));
        break;
    }

    unsigned char *raw = (unsigned char*)malloc((size_t)len);
    char *hex = (char*)malloc((size_t)len * 2 + 1);
    if (!raw || !hex) {
        if (raw) free(raw);
        if (hex) free(hex);
        free_value(lv);
        fprintf(stderr, "Out of memory in random_number\n");
        exit(1);
    }

    int ok = 0;

    /* --- Fill raw with cryptographically secure random bytes from the OS --- */
#if defined(_WIN32) || defined(_WIN64)
    {
        /* Windows: use BCryptGenRandom */
        NTSTATUS st = BCryptGenRandom(NULL, raw, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        ok = (st == 0);
    }
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    {
        arc4random_buf(raw, (size_t)len);
        ok = 1;
    }
#elif defined(__unix__)
    {
        /* Prefer getrandom if available, otherwise /dev/urandom */
        #if __has_include(<sys/random.h>)
            ssize_t n = getrandom(raw, (size_t)len, 0);
            ok = (n == (ssize_t)len);
        #else
            ok = 0;
        #endif
        if (!ok) {
            int fd = open("/dev/urandom", O_RDONLY);
            if (fd >= 0) {
                size_t off = 0; ssize_t n;
                while (off < (size_t)len && (n = read(fd, raw + off, (size_t)len - off)) > 0) off += (size_t)n;
                close(fd);
                ok = (off == (size_t)len);
            }
        }
    }
#else
    ok = 0;
#endif

    if (!ok) {
        free(raw);
        free(hex);
        free_value(lv);
        fprintf(stderr, "random_number error: OS RNG unavailable or failed\n");
        exit(1);
    }

    /* hex encode */
    static const char hexdig[] = "0123456789abcdef";
    for (int64_t i = 0; i < len; ++i) {
        unsigned char b = raw[i];
        hex[2*i]   = hexdig[(b >> 4) & 0xF];
        hex[2*i+1] = hexdig[b & 0xF];
    }
    hex[len * 2] = '\0';

    Value s = make_string(hex);
    free(raw);
    free(hex);
    free_value(lv);
    push_value(vm, s);
    break;
}
