// For scripting fun should be callable by a standard shebang line.
#!/usr/bin/env fun

/*
   This file is the creative are of an idea. Here will be fun defined before any
   implementation or programming. Just a compiled version of many ideas... ;)

   Copyright 2025 Johannes Findeisen <you@hanez.org>
   License: Apache License, Version 2.0

   Birthdate: 2025-09-10
*/

// This line MUST work as a comment.
// This is a comment! /* in combination with */ are a multiline comments.

/*
 - No ";" for line termination. New commands must be processed in a new line,
   not like in bash where you can do df; df; But you can do df; df;. :)
 - Something like printf and date formatation must exist.
 - Can be used inside ''; Like echo 'x "y" z'. ' has higher priority.
 - Intendation would be nice if would be fixed on 2 whitespaces. No tabs!
 - Internals like range() must not be redefined!
 - Functions and variables can not have the same name! If a function with name
   foo() exists, you can not asign a avlue to foo = value.
 - Strings must be initialized with the type they are and can not be missused.
   If a string is 1 you need to cast it to num to become a new type.
 - Strings are always qouted wit "" but can be qouted with '' too, but then you
   can use "" inside the string. ' can be used in the string too, but it than
   must be escaped with \.
 - Not Git based includes like in Go! No way! All dependencies MUST be managed
   inside the fun projects code base.
 - Global variables can not be redefined somwhere in the code. So defining a
   variable in a function that exist globally would overwrite the memory of the
   global variable. Local variables SHOULD be prefixed with _ or some other
   prefix and named to be unique. If you have a global variable i in a file you
   can not use i in that file anymore. If you try to define a global variable
   in your code, that is defined already in a library as a global var,
   execution will fail. So, keep the use of globals vars very very limited!
*/

// Includes from a system installed file. Included libs should support the
// extension .so and the .fun extension for core libraries implemented in fun,
// but can be included without the extension. This directory must support
// subdirectories. The root for this diretory is e.g. /var/lib/fun/. A file
// named fun.so and fun.fun can reside here, but the .fun file will be included
// here; The .fun file can then include a .so file with the same name, but then
// extension .so is required.
// Some examples:
#include crypt/md5
#include crypt/sha256
#include date
#include math
// Files can be included with or without the extension.
#include native/lib.fun
#include std.so
#include network
#include process
#include socket
#include thread
#include time

// Includes from current working directory. The prefixed / makes the difference.
// Files here are written in fun and have the .fun extension, but can be
// included with or without the extension. This directory must support
// subdirectories.
#include some_fun_file
#include includes/some_include_file.fun

// But it should be possible for .fun like system includes. Files here are
// written in fun and have the .fun extension, but can be included without the
// extension. This directory must support subdirectories.
#include /home/hanez/fun/file.fun

// Keywords (e.g., if, for, while, f)
// Operators (+, -, *, /, =, ==)
// Comparisons (==, !=, >, <)

// Variable names can contain chars and numbers from a_z, A_Z and 0_9, but can
// not start with a number.

// Boolean variables should be true/false and 1/0
boolean funny = true

// fun has a byte variable where raw bytes can bes stored.
byte mario = 0x23
// Strings need '.
byte luigi = "Jehaa!"
// Numbers don't use ' here.
byte todd = 42

float new_float = 0.12345

// Global variables can be used everywhere in the code. Even in included files.
// It should be possible to use ' and ", but ' has higher priority so " can be
// used inside '.
global string a_string = "Hello"
global string b_string = 'World!'

// This produces: Hello World!
string x_string = a_string + " " + b_string
// This produces: Hello World!
string y_string = a_string + ' ' + b_string
// This produces: Hello " " World!
string z_string = a_string + '" "' + b_string
// This produces an error.
string z_string = a_string + "' '" + b_string


// 64 bit number
number foo = 23
// Private variables can only be used here in this file.
private number bar = 42

for i in range(1, 1000)
  print i
  print add(i, 1)

for i in range(1, foo)
  print "i"

// This must not work since range() is an internal function.
string range = "f987458n73v03258"

if(x != y)
  print x
else if(a == b || h != i)
  print [a + b + h + i]
else
  if(k < 1 && l > 1)
    print "Buh!"
  fi
fi

// Internal functions.
add(a, b)
  return [a + b]

sub(a, b)
  return [a - b]

// a can be "df -h", "df -h; df -h" or "cat /etc/resolv.conf | grep nameserver".
system(a)
  // Execute a system command here and then return the return code.
  return [a]

// Some kind of cast function for converting data types.
cast(a)
  // Not implemented.
  return CASTED_variable_from_one_to_another_existing_type(a)

// External functions need f as a prefix to define the function.
f get_string(a_string, b_string)
  return a_string + " " + b_string
eof

