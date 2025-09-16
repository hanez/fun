#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://hanez.org/project/fun/
 *
 * Copyright 2025 Johannes Findeisen <you@hanez.org>
 * Licensed under the terms of the ISC license.
 * https://opensource.org/license/isc-license-txt
 */

// file I/O (writes then reads)
path = "/tmp/fun_io_example.txt"
ok = write_file(path, "hello-io")
print(ok)                        // -> 1
content = read_file(path)
print(content)                   // -> hello-io

/* Expected output:
1
hello-io
*/
