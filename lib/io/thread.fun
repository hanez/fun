/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen
 * Licensed under the terms of the Apache-2.0 license.
 * https://opensource.org/license/apache-2-0
 *
 * Added: 2025-10-04
 */

/*
 * Thread utilities built on thread_spawn() and thread_join().
 *
 * Methods:
 *   spawn(func, args) -> thread id number (args may be a single value or an array of values)
 *   join(id) -> returns the thread function's return value
 *
 * Convenience aliases:
 *   start(func, args) -> same as spawn()
 *   wait(id) -> same as join()
 */

class Thread()
  // Spawn a new thread running function `func` with `args`.
  // `args` can be a single argument or an array of arguments for the function.
  fun spawn(this, func, args)
    return thread_spawn(func, args)

  // Join a thread by id and get its return value.
  fun join(this, id)
    return thread_join(id)

  // Aliases for readability
  fun start(this, func, args)
    return thread_spawn(func, args)

  fun wait(this, id)
    return thread_join(id)
