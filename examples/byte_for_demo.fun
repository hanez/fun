#!/usr/bin/env fun

// Byte + for-loop demonstration

print("=== byte with hex literal and clamping ===")

byte b = 0x1223       // 0x1223 -> 0x23 after 8-bit clamp
print(b)              // -> 35
b = 0xFF
print(b)              // -> 255
b = 0x1FF            // 0x1FF -> 0xFF after clamp
print(b)              // -> 255

print("")
print("=== for-loop reassigning a byte variable (shows clamping near the top end) ===")

byte acc = 0
for i in range(250, 260)
  acc = i           // will clamp at 255
  print(acc)

print("")
print("=== dynamic vs typed ===")

x = 0xAB
print(typeof(x))     // -> "Number"
x = "now string"     // dynamic variable can change type
print(typeof(x))     // -> "String"

// Typed byte must remain numeric (uint8):
// b = "oops"        // Uncomment to see a runtime type error
