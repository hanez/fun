#!/usr/bin/env python3
import re
import sys
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BYTECODE = ROOT / "src" / "bytecode.h"
VM_C = ROOT / "src" / "vm.c"

OP_RE = re.compile(r'\bOP_([A-Z0-9_]+)\b')
INCLUDE_RE = re.compile(r'#include\s+"vm_case_([a-z0-9_]+)\.inc"')

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
    incs = set(m.group(1) for m in INCLUDE_RE.finditer(text))
    # Convert include base names back to uppercase opcode tokens heuristically.
    mapping = {
        "nop":"NOP","halt":"HALT",
        "load_const":"LOAD_CONST","load_local":"LOAD_LOCAL","store_local":"STORE_LOCAL",
        "load_global":"LOAD_GLOBAL","store_global":"STORE_GLOBAL",
        "pop":"POP","dup":"DUP","swap":"SWAP",
        "call":"CALL","return":"RETURN","print":"PRINT","jump":"JUMP","jump_if_false":"JUMP_IF_FALSE",
        "add":"ADD","sub":"SUB","mul":"MUL","div":"DIV",
        "mod":"MOD","lt":"LT","lte":"LTE","gt":"GT","gte":"GTE",
        "eq":"EQ","neq":"NEQ","and":"AND","or":"OR","not":"NOT",
        "make_array":"MAKE_ARRAY","len":"LEN",
        "index_get":"INDEX_GET","index_set":"INDEX_SET",
        "arr_push":"ARR_PUSH","arr_pop":"ARR_POP","arr_set":"ARR_SET","arr_insert":"ARR_INSERT","arr_remove":"ARR_REMOVE",
        "slice":"SLICE",
        "to_number":"TO_NUMBER","to_string":"TO_STRING",
        "split":"SPLIT","join":"JOIN","substr":"SUBSTR","find":"FIND",
        "enumerate":"ENUMERATE","zip":"ZIP",
        "min":"MIN","max":"MAX","clamp":"CLAMP","abs":"ABS","pow":"POW",
        "random_seed":"RANDOM_SEED","random_int":"RANDOM_INT",
        "make_map":"MAKE_MAP","keys":"KEYS","values":"VALUES","has_key":"HAS_KEY",
        "read_file":"READ_FILE","write_file":"WRITE_FILE",
    }
    tokens = set()
    for inc in incs:
        tokens.add(mapping.get(inc, inc.upper()))
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
