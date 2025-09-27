#!/usr/bin/env fun

// Environment variables via env(name):
// - Returns the value as a string.
// - If the variable is not set, returns an empty string.

print("HOME=" + env("HOME"))
print("SHELL=" + env("SHELL"))
print("FUN_NOT_SET=" + env("FUN_NOT_SET"))

// You can use it in scripts, e.g.:
fun greet()
  user = env("USER")
  if user == ""
    print("Hello, mysterious friend!")
  else
    print("Hello, " + user + "!")
greet()
