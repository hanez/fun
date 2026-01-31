/*
* This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-29
 */

/*
 * Minimal C++ opcode for Fun VM — cpp_add
 * Build is gated by -DFUN_WITH_CPP=ON (see CMake).
 */

#include <cstdint>

extern "C" {
#include "vm.h" // C header; provides VM, vm_pop_i64, vm_push_i64
}

extern "C" int fun_op_cpp_add(VM *vm) {
    int64_t a = vm_pop_i64(vm);
    int64_t b = vm_pop_i64(vm);
    vm_push_i64(vm, a + b);
    return 0; // success
}
