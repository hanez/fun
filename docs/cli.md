# Fun CLI

Reference for the `fun` command-line interface.

## Synopsis
```
fun [options] <script.fun> [-- args...]
```

If no script is supplied and interactive mode is available, `fun` starts a REPL (see [repl.md](./repl.md)).

## Common options
- `-i`, `--repl` - start an interactive REPL
- `-v`, `--version` - print version and exit
- `-h`, `--help` - show help and exit

Options may vary between versions; run `fun --help` to see what your build supports.

## Exit codes
- `0` - success
- non-zero - error during parse, compile, or runtime

## Includes and library paths
- `FUN_LIB_DIR` - environment variable that points to the stdlib location; when running from the repo, set this to `./lib`.
- `DEFAULT_LIB_DIR` - compiled-in fallback path determined at build/install time.

See also: [includes.md](./includes.md) for namespaced includes and search order.

## Examples
Run a script:
```
FUN_LIB_DIR=./lib ./build/fun examples/hello.fun
```

Start the REPL:
```
./build/fun -i
```
