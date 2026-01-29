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

// Use an FFI-safe opaque handle for the VM pointer
pub type Vm = core::ffi::c_void;

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

// Print a C string via libc printf to stdout
#[no_mangle]
pub extern "C" fn fun_rust_print_string(msg: *const core::ffi::c_char) -> i32 {
    unsafe {
        extern "C" {
            fn printf(fmt: *const core::ffi::c_char, ...) -> i32;
        }
        let fmt = b"%s\n\0".as_ptr() as *const core::ffi::c_char;
        printf(fmt, msg);
    }
    0
}

// Return a newly allocated duplicate of the given C string.
// Caller (C side) must free using fun_rust_string_free.
#[no_mangle]
pub extern "C" fn fun_rust_echo_string(input: *const core::ffi::c_char) -> *mut core::ffi::c_char {
    use core::ffi::CStr;
    unsafe {
        if input.is_null() {
            // allocate empty string
            let v = Vec::<u8>::from([0u8]);
            return v.leak().as_mut_ptr() as *mut core::ffi::c_char;
        }
        match CStr::from_ptr(input).to_str() {
            Ok(s) => {
                let mut v = s.as_bytes().to_vec();
                v.push(0); // NUL-terminate
                v.leak().as_mut_ptr() as *mut core::ffi::c_char
            }
            Err(_) => {
                // On invalid UTF-8, still duplicate bytes as-is
                let c = CStr::from_ptr(input);
                let mut v = c.to_bytes().to_vec();
                v.push(0);
                v.leak().as_mut_ptr() as *mut core::ffi::c_char
            }
        }
    }
}

// Free a C string previously returned by fun_rust_echo_string
#[no_mangle]
pub extern "C" fn fun_rust_string_free(ptr: *mut core::ffi::c_char) {
    if ptr.is_null() { return; }
    unsafe {
        // Reconstruct a Vec<u8> so Rust will free it when it drops
        // Determine length by scanning for NUL
        let mut len: usize = 0;
        while *ptr.add(len) != 0 { len += 1; }
        let slice = core::slice::from_raw_parts_mut(ptr as *mut u8, len + 1);
        let _ = Vec::from_raw_parts(slice.as_mut_ptr(), len + 1, len + 1);
    }
}

// No custom panic handler; use std default
