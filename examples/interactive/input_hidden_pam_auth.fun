#!/usr/bin/env fun

/*
 * This file is part of the Fun programming language.
 * https://fun-lang.xyz/
 *
 * Example: Demonstrate hidden input (no echo) for passwords using
 * /usr/bin/chkpwd from https://git.xw3.org/hanez/chkusr for PAM authetication.
 *
 * Added: 2026-01-02
 */

#include <io/console.fun>

print("=== input_hidden() example with PAM authentication ===")

c = Console()

username = c.ask("Username")
password = c.ask_hidden("Password")

// Do not print the password; just show the username
print(join(["Hello, ", username, "!"], ""))

res = proc_run("chkpwd -u " + username + " -p " + password)
content = res["out"]
if (res["code"] == 0)
  print("Login success!")
else
  print("Login failed!")

// Propagate the child process exit status to the Fun program's exit code
exit res["code"]

/* Possible output:
Username: hanez
Password:
=== input_hidden() example with PAM authentication ===
Hello, hanez!
Login success!

Or:
Username: hanez
Password: 
=== input_hidden() example with PAM authentication ===
Hello, hanez!
Login failed!
*/
