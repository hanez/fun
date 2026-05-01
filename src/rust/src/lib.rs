/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 */

/**
 * Rust FFI entry points and helpers used by the Fun VM (C).
 *
 * This crate builds a static library that exposes a handful of C-ABI
 * functions callable by the C VM. The functions operate on an opaque VM
 * pointer and follow the same conventions as native C opcode handlers:
 * they return 0 on success and non‑zero on error, and they communicate
 * values via VM stack helpers provided on the C side.
 *
 * Safety
 * - All extern "C" functions are inherently unsafe due to raw pointers.
 * - Callers must pass valid VM pointers provided by the C runtime.
 * - Any strings returned across the FFI boundary are NUL‑terminated and
 *   must be freed using the dedicated free function documented below.
 */

/// Use an FFI‑safe opaque handle for the VM pointer owned by the C VM.
///
/// The Rust code treats this as an opaque blob and never dereferences it
/// directly, relying instead on C helper functions exposed via FFI.
pub type Vm = core::ffi::c_void;

extern "C" {
    /// Pop a 64‑bit integer from the VM stack.
    ///
    /// Safety: `vm` must be a valid pointer to a VM instance.
    fn vm_pop_i64(vm: *mut Vm) -> i64;
    /// Push a 64‑bit integer onto the VM stack.
    ///
    /// Safety: `vm` must be a valid pointer to a VM instance.
    fn vm_push_i64(vm: *mut Vm, v: i64);
}

/// Submodule with additional Rust VM math ops (exported via C ABI).
pub mod vm;

/// Add the top two integers on the VM stack.
///
/// Stack effect
/// - Before: [..., a, b]
/// - After:  [..., a+b]
///
/// Return value
/// - 0 on success; non‑zero on error (never used here).
///
/// Safety: `vm` must be a valid VM pointer.
#[no_mangle]
pub extern "C" fn fun_op_radd(vm: *mut Vm) -> i32 {
    unsafe {
        let b = vm_pop_i64(vm);
        let a = vm_pop_i64(vm);
        vm_push_i64(vm, a + b);
    }
    0
}

/// Return a static NUL‑terminated greeting string owned by Rust.
///
/// The returned pointer remains valid for the duration of the process and
/// must NOT be freed by the caller.
///
/// Return const char* to a constant "Hello from Rust ops!" string.
#[no_mangle]
pub extern "C" fn fun_rust_get_string() -> *const core::ffi::c_char {
    b"Hello from Rust ops!\0".as_ptr() as *const _
}

/// Print a C string to stdout using libc printf("%s\n").
///
/// - If `msg` is NULL, behavior is undefined (printf will likely crash).
/// - This is intended as a simple demo and not for performance‑critical use.
///
/// Param msg NUL‑terminated UTF‑8/bytes C string.
/// Return 0 on success.
///
/// Safety: `msg` must be a valid C string pointer when non‑NULL.
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

/// Duplicate a C string and return an owned copy allocated by Rust.
///
/// - On NULL input, returns an allocated empty string ("\0").
/// - On invalid UTF‑8, the raw byte sequence is duplicated as‑is.
/// - Memory ownership is transferred to the caller, who must free it with
///   fun_rust_string_free().
///
/// Param input NUL‑terminated C string (may be NULL).
/// Return Newly allocated NUL‑terminated C string owned by the caller.
///
/// Safety: `input` must be a valid pointer if non‑NULL.
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

/// Free a C string allocated by fun_rust_echo_string().
///
/// The pointer may be NULL, in which case the function is a no‑op.
///
/// Param ptr Pointer returned by fun_rust_echo_string().
///
/// Safety: `ptr` must have been allocated by fun_rust_echo_string().
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

// No custom panic handler; use std default.
