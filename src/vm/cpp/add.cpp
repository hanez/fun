/**
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * @file add.cpp
 * @brief Fun VM C++ opcode snippet: add two 64-bit integers (cpp_add).
 *
 * This opcode is compiled and linked only when the CMake option
 * `-DFUN_WITH_CPP=ON` is enabled. It demonstrates how to implement a
 * VM opcode in C++ while exposing a C ABI symbol for the VM dispatcher.
 *
 * Stack behavior:
 * - Pops: b:int64, a:int64
 * - Pushes: (a + b):int64
 *
 * Error handling and type conversions (e.g., from other numeric types to
 * int64) are delegated to the VM helpers `vm_pop_i64` and `vm_push_i64`.
 */

#include <cstdint>

extern "C" {
#include "vm.h" // C header; provides VM, vm_pop_i64, vm_push_i64
}

/**
 * @brief Add two 64-bit integers from the VM stack and push the sum.
 *
 * Pops two values from the VM stack using `vm_pop_i64`, adds them as
 * 64-bit signed integers, and pushes the result via `vm_push_i64`.
 *
 * Stack effect:
 * - Input:  [..., a:int64, b:int64]
 * - Output: [..., (a+b):int64]
 *
 * @param vm Pointer to the VM instance. Must not be NULL.
 * @return 0 on success. Any stack underflow or conversion errors are
 *         handled by the VM helpers; non-zero may be used by future
 *         implementations to indicate a runtime error.
 */
extern "C" int fun_op_cpp_add(VM *vm) {
  int64_t a = vm_pop_i64(vm);
  int64_t b = vm_pop_i64(vm);
  vm_push_i64(vm, a + b);
  return 0; // success
}
