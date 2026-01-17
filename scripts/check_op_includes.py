#!/usr/bin/env python3

"""
This file is part of the Fun programming language.
https://fun-lang.xyz/

Copyright 2025 Johannes Findeisen <you@hanez.org>
Licensed under the terms of the Apache-2.0 license.
https://opensource.org/license/apache-2-0
"""

import re
import sys
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BYTECODE = ROOT / "src" / "bytecode.h"
VM_C = ROOT / "src" / "vm.c"

OP_RE = re.compile(r'\bOP_([A-Z0-9_]+)\b')
# Capture optional subdirectory and the basename separately
INCLUDE_RE = re.compile(r'#include\s+"vm/(?:([a-z0-9_]+)/)?([a-z0-9_]+)\.c"')

def read_text(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except Exception as e:
        print(f"error: cannot read {path}: {e}", file=sys.stderr)
        sys.exit(2)

def parse_opcodes_from_bytecode(text: str) -> set[str]:
    # Gather OP_* from the enum definition, excluding duplicates in comments
    ops = set()
    # crude: find typedef enum block and extract OP_* tokens
    enum_start = text.find("typedef enum")
    if enum_start == -1:
        enum_start = 0
    enum_block = text[enum_start:]
    for m in OP_RE.finditer(enum_block):
        token = m.group(1)
        ops.add(token)
    return ops

def parse_includes_from_vm(text: str) -> set[str]:
    pairs = [(m.group(1) or "", m.group(2)) for m in INCLUDE_RE.finditer(text)]

    # Drop support includes that are not opcode handlers
    support_includes = {"thread_common", "stubs", "handles"}
    pairs = [(d, n) for (d, n) in pairs if n not in support_includes]

    # Base-name overrides (dir-agnostic) for a few special cases
    base_overrides = {
        # core
        "nop": "NOP", "halt": "HALT",
        "load_const": "LOAD_CONST", "load_local": "LOAD_LOCAL", "store_local": "STORE_LOCAL",
        "load_global": "LOAD_GLOBAL", "store_global": "STORE_GLOBAL",
        "pop": "POP", "dup": "DUP", "swap": "SWAP",
        "call": "CALL", "return": "RETURN", "print": "PRINT",
        "jump": "JUMP", "jump_if_false": "JUMP_IF_FALSE",
        "line": "LINE",
        # arithmetic/logic
        "add": "ADD", "sub": "SUB", "mul": "MUL", "div": "DIV", "mod": "MOD",
        "lt": "LT", "lte": "LTE", "gt": "GT", "gte": "GTE",
        "eq": "EQ", "neq": "NEQ", "and": "AND", "or": "OR", "not": "NOT",
        # arrays and slices
        "make_array": "MAKE_ARRAY", "len": "LEN",
        "index_get": "INDEX_GET", "index_set": "INDEX_SET",
        "push": "PUSH", "apop": "APOP", "set": "SET", "insert": "INSERT", "remove": "REMOVE",
        "slice": "SLICE", "clear": "CLEAR", "contains": "CONTAINS", "index_of": "INDEX_OF",
        # conversions and type/meta
        "to_number": "TO_NUMBER", "to_string": "TO_STRING",
        "cast": "CAST", "typeof": "TYPEOF", "uclamp": "UCLAMP", "sclamp": "SCLAMP",
        # strings and iteration helpers
        "split": "SPLIT", "join": "JOIN", "substr": "SUBSTR", "find": "FIND",
        "enumerate": "ENUMERATE", "zip": "ZIP",
        # maps and I/O
        "make_map": "MAKE_MAP", "keys": "KEYS", "values": "VALUES", "has_key": "HAS_KEY",
        "read_file": "READ_FILE", "write_file": "WRITE_FILE",
        # os/env
        "env": "ENV", "sleep_ms": "SLEEP_MS",
        # math / RNG
        "min": "MIN", "max": "MAX", "clamp": "CLAMP", "abs": "ABS", "pow": "POW",
        "random_seed": "RANDOM_SEED", "random_int": "RANDOM_INT",
        # bitwise and shifts/rotates
        "band": "BAND", "bor": "BOR", "bxor": "BXOR", "bnot": "BNOT",
        "shl": "SHL", "shr": "SHR",
        "rol": "ROTL", "ror": "ROTR",
        # threads
        "thread_spawn": "THREAD_SPAWN", "thread_join": "THREAD_JOIN",
    }

    def map_token(d: str, n: str) -> str:
        # Directory-specific namespaces
        if d == "curl":
            return f"CURL_{n.upper()}"
        if d == "ini":
            if n in {"load", "free", "get_string", "get_int", "get_double", "get_bool", "set", "unset", "save"}:
                return f"INI_{n.upper()}"
        if d == "json":
            if n in {"parse", "stringify", "from_file", "to_file"}:
                return f"JSON_{n.upper()}"
        if d == "xml":
            if n in {"parse", "root", "name", "text"}:
                return f"XML_{n.upper()}"
        if d == "sqlite":
            if n in {"open", "close", "exec", "query"}:
                return f"SQLITE_{n.upper()}"
        if d == "libsql":
            if n in {"open", "close", "exec", "query"}:
                return f"LIBSQL_{n.upper()}"
        if d == "pcsc":
            if n in {"establish", "release", "list_readers", "connect", "disconnect", "transmit"}:
                return f"PCSC_{n.upper()}"
        if d == "pcre2":
            if n in {"test", "match", "findall"}:
                return f"PCRE2_{n.upper()}"
        if d == "tk":
            # wm_title already uses underscore
            return f"TK_{n.upper()}"
        if d == "notcurses":
            return f"NC_{n.upper()}"
        if d == "os":
            # special cases in OS
            if n == "list_dir":
                return "OS_LIST_DIR"
            if n.startswith("socket_"):
                rest = n.split("socket_", 1)[1].upper()
                return f"SOCK_{rest}"
            if n.startswith("serial_"):
                # serial_open/config/send/recv/close
                return f"SERIAL_{n.split('serial_',1)[1].upper()}"
        # Fallbacks: known base overrides, else uppercase name
        return base_overrides.get(n, n.upper())

    tokens = set(map(lambda p: map_token(p[0], p[1]), pairs))
    return tokens

def main() -> int:
    ap = argparse.ArgumentParser(description="Check that vm_case_*.inc includes cover OP_* opcodes.")
    ap.add_argument("-v", "--verbose", action="store_true", help="print summary of ops and includes")
    args = ap.parse_args()

    bc_text = read_text(BYTECODE)
    vm_text = read_text(VM_C)

    ops = parse_opcodes_from_bytecode(bc_text)
    inc_tokens = parse_includes_from_vm(vm_text)

    missing_handlers = sorted([op for op in ops if op not in inc_tokens])
    extra_includes = sorted([t for t in inc_tokens if t not in ops])

    ok = True
    if missing_handlers:
        ok = False
        print("Missing vm_case includes for opcodes:", ", ".join(missing_handlers))
    if extra_includes:
        ok = False
        print("Includes without matching OP_* in bytecode.h:", ", ".join(extra_includes))

    if ok:
        if args.verbose:
            print(f"OK: vm_case includes cover OP_* set from bytecode.h")
            print(f"  OP_* count     : {len(ops)}")
            print(f"  includes count : {len(inc_tokens)}")
            print(f"  OP_* tokens    : {', '.join(sorted(ops))}")
            print(f"  include tokens : {', '.join(sorted(inc_tokens))}")
        else:
            print("OK: vm_case includes cover OP_* set from bytecode.h")
        return 0
    return 1

if __name__ == "__main__":
    sys.exit(main())
