/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-01-27
 */

//! Rust VM helpers and opcode handlers exposed via C ABI.
//!
//! Functions in this module are compiled into the Rust static library and can
//! be called from the C VM. They operate on the VM stack using the minimal C
//! ABI helpers declared on the C side (`vm_pop_i64`, `vm_push_i64`).

use super::Vm;

extern "C" {
    fn vm_pop_i64(vm: *mut Vm) -> i64;
    fn vm_push_i64(vm: *mut Vm, v: i64);
}

/// Multiply the top two integers on the VM stack.
///
/// Before: [..., a, b]
/// After:  [..., a*b]
#[no_mangle]
pub extern "C" fn fun_op_rmul(vm: *mut Vm) -> i32 {
    unsafe {
        let b = vm_pop_i64(vm);
        let a = vm_pop_i64(vm);
        vm_push_i64(vm, a.saturating_mul(b));
    }
    0
}

/// Subtract the top two integers on the VM stack.
///
/// Before: [..., a, b]
/// After:  [..., a-b]
#[no_mangle]
pub extern "C" fn fun_op_rsub(vm: *mut Vm) -> i32 {
    unsafe {
        let b = vm_pop_i64(vm);
        let a = vm_pop_i64(vm);
        vm_push_i64(vm, a.wrapping_sub(b));
    }
    0
}

/// Integer division of the top two integers on the VM stack (a / b).
/// Pushes 0 and returns 1 when dividing by zero.
///
/// Before: [..., a, b]
/// After:  [..., a/b]
#[no_mangle]
pub extern "C" fn fun_op_rdiv(vm: *mut Vm) -> i32 {
    unsafe {
        let b = vm_pop_i64(vm);
        let a = vm_pop_i64(vm);
        if b == 0 {
            vm_push_i64(vm, 0);
            return 1; // indicate error (div by zero)
        }
        vm_push_i64(vm, a / b);
    }
    0
}
