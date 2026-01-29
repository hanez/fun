/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
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
//use core::mem::size_of;
use core::ptr;

extern "C" {
    fn vm_pop_i64(vm: *mut Vm) -> i64;
    fn vm_push_i64(vm: *mut Vm, v: i64);
    fn vm_as_mut_ptr(vm: *mut Vm) -> *mut core::ffi::c_void;
    fn vm_sizeof() -> usize;
    fn vm_value_sizeof() -> usize;
    fn vm_offset_of_exit_code() -> usize;
    fn vm_offset_of_sp() -> usize;
    fn vm_offset_of_stack() -> usize;
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

/// Demo: push current stack pointer (vm.sp) using raw offset access.
#[no_mangle]
pub extern "C" fn fun_op_rget_sp(vm: *mut Vm) -> i32 {
    unsafe {
        let base = vm_as_mut_ptr(vm) as *mut u8;
        let off_sp = vm_offset_of_sp();
        let sp_ptr = base.add(off_sp) as *const i32;
        let sp = ptr::read(sp_ptr);
        vm_push_i64(vm, sp as i64);
    }
    0
}

/// Demo: set vm.exit_code to a value popped from the stack (i64 -> i32 cast).
#[no_mangle]
pub extern "C" fn fun_op_rset_exit(vm: *mut Vm) -> i32 {
    unsafe {
        let code = vm_pop_i64(vm) as i32;
        let base = vm_as_mut_ptr(vm) as *mut u8;
        let off_ec = vm_offset_of_exit_code();
        let ec_ptr = base.add(off_ec) as *mut i32;
        ptr::write(ec_ptr, code);
    }
    0
}
