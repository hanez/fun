# Includes in Fun: local vs. system and the FUN_LIB_DIR environment variable

This document explains how to use local and system includes in Fun source files and how `FUN_LIB_DIR` controls where system includes are resolved from. It also covers namespaced includes with `as`.

## Quick summary

- Local includes: `#include "relative/path/to/file.fun"`
  - Resolved relative to the current working directory (`$PWD`) where you run `fun`.
- System includes: `#include <path/inside/stdlib.fun>`
  - Resolved using:
    1) `FUN_LIB_DIR` (if set)
    2) `DEFAULT_LIB_DIR` (compile-time default; typically `/usr/share/fun/lib/` after install)
    3) Fallback to `lib/` under the current working directory (developer convenience)
- Optional namespacing: `#include <...> as ns` or `#include "..." as ns`

## Local includes (quoted)

Use double quotes to include files relative to the directory you execute `fun` from.

Example:
```
#include "examples/include_local_util.fun"

print("== include local demo ==")
greet("Fun")

number a = 2
number b = 3
print("sum(" + to_string(a) + ", " + to_string(b) + ") = " + to_string(sum(a, b)))
```

- Resolution rule: quoted includes are read directly from the given path relative to `$PWD`.
- Typical use: including helper modules that live within your project tree.

## System includes (angle brackets)

Use angle brackets to include modules from the Fun standard library or any library directory you point `FUN_LIB_DIR` to.

Example:
```
#include <hello.fun>
#include <utils/math.fun>

print("== include lib demo ==")
hello_lib()

number x = 10
number y = 32
print("add(" + to_string(x) + ", " + to_string(y) + ") = " + to_string(add(x, y)))
print("times(" + to_string(x) + ", " + to_string(y) + ") = " + to_string(times(x, y)))
```

Resolution order for `#include <...>`:

1) `FUN_LIB_DIR` (environment variable), with automatic handling of trailing `/` or `\`
2) `DEFAULT_LIB_DIR` (compile-time define)
3) `lib/` under the current working directory (developer fallback)

If the file cannot be read from any location, an "Include error" is printed with the last attempted path.

## Namespaced includes with `as`

You can import a module into a namespace to avoid symbol collisions or to make intent explicit.

Examples:
```
// Import stdlib helpers under alias 'm'
#include <utils/math.fun> as m
print("m.add(2, 3) = " + to_string(m.add(2, 3)))
print("m.times(4, 5) = " + to_string(m.times(4, 5)))

// Import a local module into alias 'mod'
#include "examples/namespaced_mod.fun" as mod
print(mod.hello("Fun"))
g = mod.Greeter("Hi")
g.say("World")
```

Rules:
- `as` must be followed by a valid identifier (letters, digits, underscore, starting with a letter or underscore).
- Works for both local (`"..."`) and system (`<...>`) includes.

## The FUN_LIB_DIR environment variable

`FUN_LIB_DIR` overrides where the interpreter looks for angle-bracket includes. It is the first location checked when resolving `#include <...>`.

- When running directly from the repository without installing:
  - Linux/macOS/FreeBSD:
    ```
    FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/include_namespace.fun
    ```
  - Windows (CMD):
    ```
    set FUN_LIB_DIR=%CD%\lib && build-debug\fun.exe examples\include_namespace.fun
    ```
  - Windows (PowerShell):
    ```
    $env:FUN_LIB_DIR="$PWD\lib"; .\build\fun.exe examples\include_namespace.fun
    ```

- After installation:
  - You usually do not need `FUN_LIB_DIR` because `DEFAULT_LIB_DIR` points to the installed stdlib (e.g., `/usr/share/fun/lib/` on Linux). You can still set `FUN_LIB_DIR` to test custom or alternate library trees.

- Interaction with `DEFAULT_LIB_DIR`:
  - If `FUN_LIB_DIR` is not set or is empty, the interpreter tries `DEFAULT_LIB_DIR` (a compile-time value). Build systems may set this via `-DDEFAULT_LIB_DIR=...`. The project’s CMake sets sensible OS-specific defaults.

- Developer fallback:
  - If neither `FUN_LIB_DIR` nor `DEFAULT_LIB_DIR` resolves the include, the interpreter finally tries `lib/<path>` relative to `$PWD`.

Tips:
- To verify which stdlib is being used, temporarily include a file you control and log from it.
- Mind your current directory: running `fun` from a different folder changes how quoted includes resolve and can change the `lib/` fallback for angle-bracket includes.

## Common patterns and best practices

- Prefer quoted includes for project-internal modules: `#include "src/mods/foo.fun"`
- Prefer angle brackets for stdlib and external libs: `#include <utils/math.fun>`
- Use `as` for clarity and to avoid name clashes: `#include <utils/math.fun> as math`
- When developing against the repo without installing, export `FUN_LIB_DIR=$(pwd)/lib` before running examples or your app.
- Keep paths portable: avoid OS-specific separators inside include paths; the resolver handles path joining.

## Troubleshooting include errors

- Error: `Include error: cannot read '...'`
  - Check if the path exists in the intended location:
    - For `"..."`: is the path correct relative to your current working directory?
    - For `<...>`: does the file exist under `FUN_LIB_DIR/<...>` or `DEFAULT_LIB_DIR/<...>` or `./lib/<...>`?
  - Ensure `FUN_LIB_DIR` is exported in the same shell session where you run `fun`.
  - On Windows, verify you used the correct syntax for setting environment variables in CMD vs. PowerShell.

## See also

- Examples in this repository:
  - `examples/include_local.fun`
  - `examples/include_lib.fun`
  - `examples/include_namespace.fun`
- REPL notes: `docs/repl.md` (explains completion that also scans `FUN_LIB_DIR`)
