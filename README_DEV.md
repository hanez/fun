# Developer Notes

This project splits VM opcode handlers into one-include-per-op file for readability.
The main interpreter loop (`src/vm.c`, `vm_run`) includes `vm_case_*.inc` files,
each containing a single `case OP_*: { ... }` handler.

## Adding a new opcode

1. Add the enum in `src/bytecode.h` (e.g., `OP_FOO`) and add its human-readable name
   to `opcode_names[]` in `src/vm.h`. Update `opcode_is_valid` upper bound if needed.
2. Implement the VM handler in `src/vm_case_foo.inc` using the established style:
   - No function wrappers, just `case OP_FOO: { ... break; }`.
3. Include the new file in the switch inside `src/vm.c` (`vm_run`), near similar ops.
4. Run the checker script:
