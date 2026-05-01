/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file pcre2.c
 * @brief PCRE2 configuration header and includes for regex-related opcodes.
 *
 * Ensures PCRE2 code unit width is defined consistently prior to including
 * <pcre2.h> when FUN_WITH_PCRE2 is enabled.
 */

/* Ensure PCRE2 is configured consistently across the whole translation unit.
 * vm.c includes many opcode implementation .c files; some use PCRE2. For PCRE2
 * headers to expose the correct typedefs (e.g., pcre2_code, PCRE2_SPTR), the
 * PCRE2_CODE_UNIT_WIDTH macro must be defined before the first inclusion of
 * <pcre2.h>. We do this once here when PCRE2 support is enabled. */
#ifdef FUN_WITH_PCRE2
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
#endif
