#!/usr/bin/env fun

// Signed integer type examples with two's complement wrapping

int8  s8  = -1       // should be -1 within int8 range
int16 s16 = -32768   // min int16
int32 s32 = -2147483648
int64 s64 = -9223372036854775808

print("s8  = " + to_string(s8)  + " :: " + typeof(s8))
print("s16 = " + to_string(s16) + " :: " + typeof(s16))
print("s32 = " + to_string(s32) + " :: " + typeof(s32))
print("s64 = " + to_string(s64) + " :: " + typeof(s64))

// Demonstrate wrapping on assignment beyond range
s8  = 130    // 130 -> wraps to -126 in int8
s16 = 70000  // wraps into int16 range
s32 = 4294967296 // wraps into int32 range
s64 = 18446744073709551616 // wraps into int64 range

print("wrap s8  -> " + to_string(s8))
print("wrap s16 -> " + to_string(s16))
print("wrap s32 -> " + to_string(s32))
print("wrap s64 -> " + to_string(s64))

// Arithmetic with signed values then wrapping
s8 = -120
s8 = s8 - 20   // -140 -> wraps into int8 range
print("s8 after -20 wrap -> " + to_string(s8))
