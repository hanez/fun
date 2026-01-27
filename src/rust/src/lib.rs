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

#![no_std]

#[repr(C)]
pub struct Vm;

extern "C" {
    fn vm_pop_i64(vm: *mut Vm) -> i64;
    fn vm_push_i64(vm: *mut Vm, v: i64);
}

// Submodule with additional Rust VM math ops (exported via C ABI)
pub mod vm;

#[no_mangle]
pub extern "C" fn fun_op_radd(vm: *mut Vm) -> i32 {
    unsafe {
        let b = vm_pop_i64(vm);
        let a = vm_pop_i64(vm);
        vm_push_i64(vm, a + b);
    }
    0
}

#[no_mangle]
pub extern "C" fn fun_rust_get_string() -> *const core::ffi::c_char {
    b"Hello from Rust ops!\0".as_ptr() as *const _
}

// Minimal panic handler for no_std; abort behavior requested via Cargo profile
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
