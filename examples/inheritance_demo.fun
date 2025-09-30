#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-01
 */

/*
 * Inheritance demo:
 * - Child extends Parent
 * - Parent initializes fields in its constructor
 * - Child overrides/extends behavior and fields
 */

print("=== inheritance demo ===")

class Parent(number start)
  value = 0
  fun _construct(this, s)
    this.value = s

  fun describe(this)
    return "Parent(value=" + to_string(this.value) + ")"

  fun hello(this)
    return "Hello from Parent()"

class Child(number start) extends Parent
  bonus = 5

  fun _construct(this, s)
    // child's constructor runs after parent's fields were merged
    this.value = this.value + this.bonus

  fun describe(this)
    return "Child(value=" + to_string(this.value) + ", bonus=" + to_string(this.bonus) + ")"

p = Parent(10)
print(p.describe())       // expect: Parent(value=10)

c = Child(10)
print(c.describe())       // expect: Child(value=15, bonus=5)
print(c.hello())

print("=== done ===")

/* Expected output:
=== inheritance demo ===
Parent(value=10)
Child(value=15, bonus=5)
Hello from Parent()
=== done ===
*/
