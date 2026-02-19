# Running the Examples

This page shows how to run the example programs included with the repository and how to use the interactive showcase script.

All commands assume you are in the repository root.

## Prerequisites

- Build the interpreter (see handbook.md). You’ll have `build/fun` (paths may vary by your setup/IDE).
- Set FUN_LIB_DIR to the repo’s lib directory when running without installation so `#include <...>` can find the standard library.

Example (Linux/macOS/BSD):

```
FUN_LIB_DIR="$(pwd)/lib" ./build/fun examples/include_lib.fun
```

Windows (PowerShell):

```
$env:FUN_LIB_DIR = "$PWD/lib"
./build/fun.exe .\examples\include_lib.fun
```

## Interactive showcase: play.fun

The script `./play.fun` discovers all `.fun` files under `./examples` and offers to run them one by one:

```
./play.fun
```

Notes:
- The script auto-picks your interpreter (FUN_BIN env or `fun` in PATH) and ensures `FUN_LIB_DIR=./lib` so examples resolve includes correctly.
- It shows the exit code for each run and summarizes failures at the end.

Tip: you can run specific examples directly too:

```
FUN_LIB_DIR="$(pwd)/lib" fun examples/crypto/openssl_md5.fun
```

## Example categories

Browse the `examples/` tree for areas of interest:

- crypto — crypto demonstrations (e.g., OpenSSL MD5 helper; requires build with `-DFUN_WITH_OPENSSL=ON`)
- blocking / interactive — I/O or user-interactive patterns
- error / broken — negative tests and error showcases
- math — numeric operations
- sqlited / data — data access and HTTP/CGI-style samples (platform dependent)

## Creating your own examples

Place your `.fun` files anywhere under `examples/` to have them picked up by `play.fun`. Use quoted includes for project-local helpers and angle brackets for stdlib modules:

```
#include "examples/my_lib/common.fun"
#include <io/console.fun>
```

If you add an example showcasing a new feature, also consider adding a brief note to the relevant doc (types.md, includes.md, opcodes.md, etc.).
