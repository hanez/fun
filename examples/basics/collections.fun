#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Copyright 2026 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2026-03-25
 */

// Arrays and maps basics

arr = [1, 2, 3]
push(arr, 4)
print("arr size=" + to_string(len(arr)) + ", last=" + to_string(arr[3]))

user = {"name": "Lin", "age": 29}
user["city"] = "Paris"
for k in keys(user)
  print(k + ": " + to_string(user[k]))

/* Expected output:
arr size=4, last=4
age: 29
name: Lin
city: Paris
*/
