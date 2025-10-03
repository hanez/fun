/**
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/**
 * @file time_now_ms.c
 * @brief Implements OP_TIME_NOW_MS to push current wall-clock time in ms since Unix epoch.
 *
 * Stack before: []
 * Stack after: [int ms]
 */

#include <time.h>
#include <stdint.h>

case OP_TIME_NOW_MS: {
    int64_t ms;
#if defined(CLOCK_REALTIME) && !defined(_WIN32)
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        ms = (int64_t)ts.tv_sec * 1000 + (int64_t)(ts.tv_nsec / 1000000);
    } else {
        time_t s = time(NULL);
        ms = (int64_t)s * 1000;
    }
#else
    time_t s = time(NULL);
    ms = (int64_t)s * 1000;
#endif
    push_value(vm, make_int(ms));
    break;
}
