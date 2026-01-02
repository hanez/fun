#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Example: Demonstrate hidden input (no echo) for passwords.
 *
 * Added: 2026-01-02
 */

#include <io/console.fun>

print("=== input_hidden() example ===")

c = Console()

username = c.ask("Username")
password = c.ask_hidden("Password")

// Do not print the password; just show the username
print(join(["Hello, ", username, "!"], ""))

/* Expected output:
Username: hanez
Password:
=== input_hidden() example ===
Hello, hanez!
*/
