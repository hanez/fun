#!/usr/bin/env fun
// The shebang line makes no sense here because this is a library which will 
// never be executed, but it shows that it is not wrong. 

/*
* This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-09-30
 */

// namespaced_mod.fun
// Simple module to demonstrate include-as namespaces with functions and classes.

fun hello(name)
  return "Hello, " + to_string(name) + "!"

class Greeter(string prefix)
  // Methods must declare 'this' as the first parameter
  fun say(this, name)
    print(this.prefix + " " + to_string(name))
