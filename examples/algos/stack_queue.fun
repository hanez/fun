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

// Array helpers
#include <arrays.fun>

// Stack via array push/pop
stack = []
push(stack, 1)
push(stack, 2)
push(stack, 3)
print("stack pop: " + to_string(stack[len(stack)-1]))
pop = stack[len(stack)-1]
// Use array_slice(arr, start, count) to drop the last element
stack = array_slice(stack, 0, len(stack)-1)
print("stack now: " + to_string(stack))

// Queue via array push/shift
queue = []
push(queue, "a")
push(queue, "b")
push(queue, "c")
head = queue[0]
// Keep all but the first element
queue = array_slice(queue, 1, len(queue)-1)
print("queue head: " + to_string(head))
print("queue now: " + to_string(queue))

/* Expected output:
stack pop: 3
stack now: [array n=2]
queue head: a
queue now: [array n=2]
*/
