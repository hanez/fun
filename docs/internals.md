# Fun Internals

This document describes how Fun is implemented under the hood: the bytecode format, the virtual machine (VM) execution model, how opcodes are organized, error handling and debugging, and how the parser translates source code into bytecode.

It complements the Handbook and Spec by focusing on implementation details in the C core, with pointers to the relevant source files.

- VM implementation: src/vm.c (and many small opcode handlers in src/vm/...)
- Parser and compiler: src/parser.c
- Bytecode and values: src/bytecode.h, src/value.h
- VM API/structure: src/vm.h

## High‑level architecture

Fun programs are compiled from source text to a compact bytecode representation (Bytecode). The VM executes this bytecode using a simple stack machine with call frames, locals, and globals.

- Parser produces a Bytecode object per compiled unit (e.g., a module or function).
- Bytecode is a linear array of Instruction { op, operand } entries with a parallel constants table.
- The VM maintains:
  - A value stack for computation
  - A frame stack for function calls (locals, instruction pointer, try/catch state)
  - A globals array
  - An output buffer (captures printed values), plus tracing/debugger state

Almost all operations are implemented as small, focused opcode handlers. The VM’s main interpreter loop dispatches these opcodes and performs type‑aware operations on Value instances.

## Values, bytecode, and instructions

Relevant headers:
- src/value.h — the tagged value type used by the VM (ints, floats, strings, arrays, maps, nil, booleans, etc.).
- src/bytecode.h — the instruction set and bytecode container types.

Instruction set:
- enum OpCode defines all opcodes (OP_NOP, OP_LOAD_CONST, OP_ADD, …). See src/bytecode.h.
- Instruction is a pair { OpCode op; int32_t operand; }.
  - Some opcodes encode immediate arguments in operand (e.g., local slot index, constant index, jump target, arg count).

Bytecode container (Bytecode):
- instructions: dynamic array of Instruction
- constants: dynamic array of Value copies (literals, strings, numbers, etc.)
- debug metadata: name (function or module), source_file (for error mapping)

Utilities:
- bytecode_add_constant, bytecode_add_instruction, bytecode_set_operand, bytecode_dump…

## VM structure and execution model

See src/vm.h for the main VM and Frame shapes.

Frame:
- fn: pointer to the current Bytecode (function or module entry)
- ip: instruction pointer (index into instructions)
- locals[MAX_FRAME_LOCALS]: local slots for this frame
- try_stack[16], try_sp: per‑frame exception handler stack (see exceptions section)

VM:
- stack[STACK_SIZE], sp: data stack and stack pointer
- frames[MAX_FRAMES], fp: call frame stack and frame pointer
- globals[MAX_GLOBALS]: global slots
- output[OUTPUT_SIZE], output_count, output_is_partial[]: captures output of OP_PRINT/OP_ECHO
- instr_count: instructions executed during the last vm_run
- current_line: last known source line (maintained via OP_LINE)
- exit_code: set by OP_EXIT
- tracing flags and REPL‑on‑error hook
- debugger state (step/next/finish, breakpoints)

Initialization and lifecycle:
- vm_init(VM*): zeroes state and prepares stacks
- vm_reset(VM*): frees/clears dynamic state while keeping the VM instance
- vm_free(VM*): tear‑down helper
- vm_run(VM*, Bytecode* entry): pushes an initial frame and enters the interpreter loop

## The interpreter loop and opcode dispatch

The interpreter loop lives in src/vm.c: vm_run. Opcodes are executed in a tight loop that:
- Fetches the current instruction (op, operand) from the active frame (frames[fp])
- Optionally updates debug/tracing state (e.g., OP_LINE updates VM.current_line)
- Executes the handler for the opcode
- Advances ip, or jumps/returns/halts as needed

Opcode handlers organization:
- To keep vm.c readable, most opcode implementations are factored into small .c files included directly into vm.c (e.g., vm/core/load_const.c, vm/logic/and.c, vm/arrays/push.c, vm/math/abs.c, vm/os/thread_spawn.c, etc.).
- This is a deliberate “amalgamation” style: small single‑purpose C units compiled as part of vm.c.
- Optional subsystems (JSON, PCRE2, CURL, SQLite, libSQL, PC/SC, XML2, Tcl/Tk, Notcurses, INI, sockets, serial, OS helpers) are grouped under src/external and src/vm/<domain>/.

Dispatch naming and visibility:
- Human‑readable names for opcodes live in vm.h: opcode_names[]. These are used in debug prints and error messages.

## Stacks, frames, locals, globals

Data stack:
- push_value/pop_value manage stack items of type Value.
- Most opcodes pop their arguments (right‑to‑left) and push a result.

Call frames:
- vm_push_frame sets up a new frame for a function call (OP_CALL), transferring arguments into the callee’s local slots per the calling convention implemented by the compiler.
- vm_pop_frame unwinds one frame, restoring caller context and optionally leaving a return value on the data stack.

Locals and globals:
- OP_LOAD_LOCAL/STORE_LOCAL address MAX_FRAME_LOCALS slots in the current frame.
- OP_LOAD_GLOBAL/STORE_GLOBAL access the VM‑wide globals table.

## Control flow, calls, and returns

- OP_JUMP and OP_JUMP_IF_FALSE implement structured control flow compiled by the parser (if/elif/else, loops, conditionals).
- OP_CALL pops function + N args, sets up a callee frame, and transfers control.
- OP_RETURN unwinds the current frame. The interpreter returns from vm_run when the entry frame is popped or a HALT/EXIT is executed.

## Type system and operations

The VM is dynamically typed. Values carry a tag; operations check types at runtime and coerce where sensible (e.g., number parsing in OP_TO_NUMBER). Representative groups:

- Core arithmetic: OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, plus float‑aware rounding and transcendental ops (FLOOR/CEIL/TRUNC/ROUND, SIN/COS/TAN, EXP/LOG/LOG10/SQRT, FMIN/FMAX, GCD/LCM/ISQRT/SIGN).
- Logic and comparisons: OP_LT/LTE/GT/GTE/EQ/NEQ, OP_AND/OR/NOT.
- Stack utils: OP_DUP/OP_SWAP/OP_POP.
- Arrays: OP_MAKE_ARRAY/INDEX_GET/INDEX_SET/LEN/PUSH/APOP/SET/INSERT/REMOVE/SLICE/CONTAINS/INDEX_OF/CLEAR/ENUMERATE/ZIP.
- Strings and regex: OP_SPLIT/JOIN/SUBSTR/FIND and OP_REGEX_MATCH/SEARCH/REPLACE (and PCRE2 variants if enabled).
- Maps: OP_MAKE_MAP/KEYS/VALUES/HAS_KEY.
- Conversions/reflection: OP_TO_NUMBER/TO_STRING/CAST/TYPEOF, OP_UCLAMP/SCLAMP.
- I/O and OS: OP_READ_FILE/WRITE_FILE/INPUT_LINE/ENV/PROC_RUN/PROC_SYSTEM/TIME_NOW_MS/CLOCK_MONO_MS/DATE_FORMAT/OS_LIST_DIR/RANDOM_NUMBER, sockets, serial.
- External integrations (optional): JSON, CURL, SQLite, libSQL, PC/SC, XML2, Tcl/Tk, Notcurses, INI.

Each handler enforces argument types and returns clear error messages via vm_raise_error on misuse.

## Source lines, tracing, and error reporting

Source line tracking:
- The compiler emits OP_LINE markers carrying 1‑based source line numbers. vm_run updates VM.current_line when these execute.

Tracing:
- When VM.trace_enabled is set, vm_run prints each opcode and stack state. This is helpful for debugging compiled programs.

Enhanced error messages:
- vm.c wraps fprintf for stderr to append context: source file, line, function name, opcode, and ip of the last executed instruction.
- For sources expanded via include preprocessing, vm.c maps the line back to the included file using preprocess_includes and inline markers (see below).

## Exceptions: try/catch/finally

Minimal structured exceptions are implemented with three opcodes and a per‑frame try stack:
- OP_TRY_PUSH operand = handler ip; pushes a handler location to Frame.try_stack
- OP_TRY_POP pops the current handler
- OP_THROW pops an error value; if a handler exists in the current frame, control jumps to it with the error value available on the stack; otherwise, vm_raise_error terminates execution (or triggers REPL if configured)

This design keeps exception metadata strictly per frame and avoids VM‑global unwind state.

## REPL‑on‑error and debugger

Runtime error path:
- vm_raise_error consults the current frame’s try handlers. If none match, it formats a message with location, prints it, and sets exit_code.
- If VM.repl_on_error is enabled and a REPL hook is installed, the VM drops into the REPL to inspect state.

Debugger state (vm.h):
- Step/Next/Finish modes, breakpoints stored in VM.breakpoints[].
- vm_debug_* helpers manage breakpoints and stepping requests. vm_run consults this state at loop boundaries to pause execution.

## Include preprocessing and source mapping

Fun supports a lightweight include mechanism at the source text level (handled before/around compilation) to allow composing modules. vm.c provides two helpers used for error mapping:

- preprocess_includes(const char* src) expands the source by inlining included files and injecting marker comments of the form:
  // __include_begin__: <path>:<line>
  …included lines…
  // __include_end__: <path>:<line>

- map_expanded_line_to_include(path, line, out_path, out_line) uses these markers to map a line in the expanded text back to the original file:line that contributed it.

When stderr output is produced by the VM, fun_vm_vfprintf annotates messages with the mapped file and line if possible.

## Parser and compiler pipeline (src/parser.c)

The parser compiles directly to bytecode in a single pass with localized backpatching. The code is organized by precedence levels and statement/block parsing.

Key components:

- Namespaces and aliases: ns_aliases_scan detects alias directives at the top of the source (for module resolution) before parsing proper.
- Symbol tables: sym_index for globals; LocalEnv tracks locals in the current function scope. local_find/local_add manage local slots.
- Expression parser: a classic precedence‑climbing/recursive‑descent set of emit_* functions:
  - emit_primary: literals, identifiers, grouping, array/map literals, function literals, calls, indexing
  - emit_unary: prefix ops like !, unary -, type conversions
  - emit_multiplicative/additive/relational/equality/and/or: binary operators by precedence
  - emit_conditional: ternary/conditional constructs if supported by the grammar
  - emit_expression: entry point that threads all the above

- Statement and block parsing:
  - read_line_start and skip_to_eol implement indentation and line/whitespace/comment handling (Fun uses indentation‑based blocks).
  - parse_simple_statement emits bytecode for assignments, declarations, expression statements, control flow (if/elif/else, while/for), returns, breaks/continues, try/catch/finally constructs, print/echo, etc.
  - parse_block handles nested blocks, indentation tracking, and emits OP_LINE markers for accurate source positioning.

- Control‑flow codegen:
  - Conditional and loop constructs emit OP_JUMP/OP_JUMP_IF_FALSE with forward jump placeholders patched later via bytecode_set_operand.
  - try/catch/finally: emit OP_TRY_PUSH/OP_TRY_POP and arrange handler ips; OP_THROW for explicit throw.

- Functions and calls:
  - Functions compile to their own Bytecode with a fresh LocalEnv; callers use OP_CALL with operand = arg count. Arguments are pushed left‑to‑right; the callee consumes them from the stack into local slots as per the compiler’s calling convention.

- Constants and literals:
  - String/number/boolean/nil literals are interned into the Bytecode.constants table. OP_LOAD_CONST references them by index.

- Line information and files:
  - The parser emits OP_LINE as it advances through source lines. Bytecode.source_file is set so the VM can report accurate errors.

Front‑end entry points:
- parse_string_to_bytecode(const char* source)
- parse_file_to_bytecode(const char* path)
- compile_minimal for very small snippets/tests

These return an owned Bytecode* that the VM can execute.

## Libraries and built‑ins layout

The VM includes small, standalone C files for each feature group under src/vm/…

- Core: src/vm/core/*.c (load/store, jumps, call/return, stack ops, halt/exit, try/throw)
- Numbers and logic: src/vm/arithmetic/*.c, src/vm/logic/*.c, src/vm/bitwise/*.c, src/vm/math/*.c
- Collections and strings: src/vm/arrays/*.c, src/vm/maps/*.c, src/vm/strings/*.c
- Conversions/reflection: src/vm/*.c (to_number, to_string, cast, typeof, uclamp, sclamp)
- OS and I/O: src/vm/io/*.c, src/vm/os/*.c, sockets and serial
- External integrations: src/external/*.c glue with opcode handlers in src/vm/<domain> when enabled by CMake options

Feature flags (CMake):
- Many subsystems are guarded by -DFUN_WITH_… options (JSON, PCRE2, CURL, PCSC, SQLITE, LIBSQL, XML2, TCLTK, NOTCURSES, INI, REPL). See CMake options in the Handbook.

## Debugging and development tips

- Use the --trace flag (or VM.trace_enabled) to inspect execution step‑by‑step.
- Use OP_LINE markers (visible via bytecode_dump) to correlate bytecode with source lines.
- vm_dump_globals helps inspect non‑nil globals at runtime.
- When adding a new opcode:
  1) Extend enum OpCode and opcode_names[]
  2) Implement a handler (small C file) and include it from src/vm.c
  3) Teach the parser/emitter to generate the opcode
  4) Update docs/spec and examples

## Data limits and sizes

From vm.h defaults (tuned for simplicity; adjust if needed):
- STACK_SIZE = 1024
- MAX_FRAMES = 128
- MAX_FRAME_LOCALS = 64
- MAX_GLOBALS = 128
- OUTPUT_SIZE = 1024

## Entry points recap

- VM execution: vm_init → vm_run(entry) → vm_print_output/vm_clear_output → vm_reset/vm_free.
- Parsing: parse_string_to_bytecode / parse_file_to_bytecode → Bytecode*.
- Bytecode helpers: bytecode_add_instruction/constant, bytecode_set_operand, bytecode_dump.

## Where to look in the source

- src/vm.c — interpreter loop, error/trace, and amalgamated opcode includes
- src/vm.h — VM/Frame definitions and debugger API
- src/bytecode.h — instruction set and bytecode container
- src/parser.c — compiler, expression/statement/block parsing, emission, indentation handling
- src/vm/* — small focused opcode handlers by domain
- src/external/* — integration shims for optional dependencies

## Concurrency, isolates, and garbage collection

We chose isolated state (like Lua) rather than a single global lock (like Python’s GIL). Each VM has its own heap, scheduler, and GC. There are no cross‑VM pointers. Concurrency and data exchange happen via message passing and a few carefully scoped shared‑memory primitives for high‑throughput use cases. This keeps the C API simple, predictable, and safe to embed in multi‑threaded hosts.

### Concurrency model

- Isolates/VMs: Each `fun_vm_t*` is an isolate with its own heap, bytecode, scheduler, and GC. Multiple VMs can run truly in parallel on different OS threads/cores.
- Intra‑VM concurrency: User‑space tasks (green threads/fibers) scheduled by the VM. Within a single VM, you get structured concurrency; the VM is single‑owner from the host’s perspective.
- Inter‑VM concurrency: Use message passing (channels/ports) and zero‑copy shared buffers where explicitly opted in.

### Memory and sharing

- Per‑VM heaps: Objects are allocated, owned, and collected per VM. No object from VM A may be referenced by VM B.
- No global runtime lock: There is no GIL. VMs never contend on a global lock and can scale across cores.
- Message passing: `fun_send(port, value)` and `fun_recv(port, timeout)` copy values across VM boundaries using a compact, GC‑safe serialization format.
- Zero‑copy fast path (opt‑in): For large payloads, hosts can create `fun_shared_buffer` objects which are reference‑counted, immutable within VMs, and can be shared across VMs without copying. Mutating a shared buffer requires creating a new buffer (copy‑on‑write style), preserving safety.

### Thread‑safety rules for the C API

- VM affinity: A `fun_vm_t*` has thread affinity. Host code must interact with a VM from its owning thread. If you need to call into the same VM from multiple OS threads, you post work to the VM’s run loop via `fun_vm_post(vm, callback, user_data)`.
- Opaque handles: All handles (`fun_vm_t*`, `fun_value_t`, `fun_port_t`, `fun_shared_buffer_t`) are opaque. There are no raw pointers to VM heap objects in the API.
- No cross‑VM objects: You cannot pass `fun_value_t` directly across VMs. Use `fun_send`/`fun_serialize`/`fun_deserialize` or `fun_shared_buffer`.
- Optional locking helpers: For the rare case where a host wants shared mutable state outside the VM, we expose thin wrappers over atomics and locks (`fun_atomic_*`, `fun_mutex_t`, `fun_rwlock_t`) so extension code doesn’t need to mix threading libraries. These do not participate in GC and are outside VM heaps.

### Scheduling and GC

- Per‑VM scheduler: Green threads are multiplexed within a VM. Preemption is cooperative with periodic safe points; an optional time‑slice can yield between bytecode instruction groups when `FUN_DEBUG` or tracing is enabled.
- Per‑VM GC: Stop‑the‑world, per‑VM. No global stop‑the‑world across VMs. A GC in one VM does not pause others.

### Shared memory as the escape hatch

Message passing and serialization are the defaults. When copying becomes too expensive, use shared memory — but keep it outside the tracing heaps and very constrained:

- Off‑heap, opaque buffers: Use `fun_shared_buffer` for large blobs (e.g., images, JSON bytes, tensors). These are refcounted, live outside any VM heap, and are immutable from the VM’s perspective.
- No heap pointers inside: Shared regions must not contain pointers into any VM heap. Treat them as raw bytes plus size/stride metadata.
- Synchronization via atomics/futexes (host side): If you need concurrent producers/consumers over shared buffers, build lock‑free queues or channels on the host using atomics. The VM sees only opaque handles.
- Lifetime: Governed by explicit ownership (retain/release), epochs, or arena lifetime — not by any VM’s GC.

This provides zero‑copy hand‑off without coupling VMs or their collectors.

### Zero-Copy Shared Buffering

- We default to isolates for safety and scaling.
- Zero‑copy sharing is done with `fun_shared_buffer`, an off‑heap, GC‑untracked, pointer‑free block that’s immutable from the VM’s point of view.
- Lifetime is managed with plain reference counting (`retain/release`).
- For hot paths, we also support an adoption (ownership‑transfer) pattern during message passing so the sender can drop its ref without copying.

#### How zero‑copy is kept safe

- Off‑heap + untraced: `fun_shared_buffer` lives outside any VM heap, so per‑VM GCs never scan it and never need cross‑VM barriers.
- Immutable inside VMs: once a buffer is visible to any VM, it is treated as read‑only by that VM code. That avoids aliasing hazards and lets multiple VMs parse the same bytes concurrently.
- Pointer‑free contract: the buffer contains raw bytes only (no pointers into VM heaps), which prevents accidental cross‑heap reachability.

#### Lifetime model

- Global refcount on the shared object.
  - `fun_shared_buffer_retain`/`fun_shared_buffer_release` drive deallocation.
  - VMs just hold handles; the buffer’s memory is not traced by any VM.
- Host mutation policy:
  - You may fill/mutate the buffer before publication (while only the host holds a ref).
  - After passing it into any VM or port, treat it as immutable; further mutation must be done by creating a new buffer or by using host‑side synchronization and not exposing the mutated view to VMs that assume immutability.

#### Ownership transfer (adoption)

- Message passing can “adopt” a buffer: instead of copying, the sender passes the handle and typically `release`s its ref after send.
- The receiver `retain`s upon receipt if it needs to outlive the message scope.
- This gives you zero‑copy with a clear single‑writer → multi‑reader handoff pattern.

#### Why not share GC‑managed objects?

- Sharing traced objects would require global safepoints or a fully barriered concurrent GC across VMs, or switching to atomic RC for those objects. We avoid that entirely; only off‑heap `fun_shared_buffer` is shared.

#### Minimal API (illustrative)

```c
// Zero-copy shared buffers (immutable inside VMs)
fun_shared_buffer_t* fun_shared_buffer_new(size_t n);
void* fun_shared_buffer_data(fun_shared_buffer_t*);
void fun_shared_buffer_retain(fun_shared_buffer_t*);
void fun_shared_buffer_release(fun_shared_buffer_t*);

// Ports/channels for inter-VM comms
fun_port_t* fun_port_create(fun_vm_t*);
int fun_send(fun_port_t*, fun_value_t value);      // can carry a shared buffer handle
int fun_recv(fun_port_t*, fun_value_t* out, uint64_t timeout_ms);
```

#### Practical usage tips

- Default to isolates + ports for logic; use `fun_shared_buffer` only for large payloads (images, tensors, blobs).
- If you need shared mutability from native code, keep it outside the VM and guard with `fun_mutex_t`/`fun_rwlock_t` or `fun_atomic_*`. Don’t expose mutable state to VMs.
- Document clearly when a buffer is “published” to VMs; after that point, treat it as immutable and rely on refcounts for lifetime.

#### Bottom line

- It’s reference counting by default, with an optional ownership‑transfer handoff in message passing to avoid copies. This keeps the host API small and predictable while preserving GC isolation and safety.

### Immutable Buffer Sync via Ports

We don’t expose shared mutability to VMs. The trick is: publish‑as‑immutable plus adoption via ports. Ports/queues do the synchronization; `fun_shared_buffer` is off‑heap and refcounted with atomic ops. The host doesn’t need to lock anything for the common paths.

#### What removes the host’s synchronization burden

- Immutable after publication
  - Host fills a `fun_shared_buffer` while it’s private, then “publishes” it by sending through a port. From that point, all VM code treats it as read‑only. No data races; no host locks.
- Port/queue owns the concurrency
  - `fun_send`/`fun_recv` operate on an internal MPMC queue with the necessary atomics/fences. You don’t implement a queue nor protect it; the runtime does.
- Adoption (ownership transfer)
  - `fun_send` can adopt a buffer handle. Sender drops its ref after send; receiver retains if needed. Lifetime is clear without locks.
- Atomic refcounting only
  - The only shared mutable state is the buffer’s global refcount, updated atomically inside the runtime. You don’t touch it.
- VM thread affinity
  - You never call into a VM from arbitrary threads; use `fun_vm_post` to hop to the owning thread. This avoids host‑side marshaling races.

#### Typical pattern (no locks needed)

```c
fun_shared_buffer_t* b = fun_shared_buffer_new(n);
void* p = fun_shared_buffer_data(b);
memcpy(p, src, n);                  // fill while private

fun_send(port, fun_wrap_shared(b)); // publish + adopt
fun_shared_buffer_release(b);       // drop sender’s ref (optional if send adopts)

// On receiver side
fun_value_t v;                       
if (fun_recv(port, &v, 1000) == 0) {
  fun_shared_buffer_t* r = fun_unwrap_shared(v);
  // r is read‑only to VMs; parse/process concurrently without locks
  // Retain if it must outlive the message scope
  fun_shared_buffer_retain(r);
  …
  fun_shared_buffer_release(r);
}
```

#### When would the host ever sync?

- Only if you choose shared mutability outside the VM (e.g., a lock‑free ring buffer you manage). For that, we expose optional helpers (`fun_atomic_*`, `fun_mutex_t`, `fun_rwlock_t`), but they’re not required for the standard zero‑copy path.

#### Guarantees provided by the runtime

- Publication fences around `fun_send`/`fun_recv` ensure bytes written before send are visible to receivers without extra barriers.
- Buffers are pointer‑free relative to VM heaps, so no cross‑heap reachability or GC coordination is needed.
- Refcount updates are atomic; deallocation happens when the global count drops to zero.

#### Bottom line

- You don’t synchronize shared data: you publish immutable blobs and let ports handle concurrency and lifetime via atomic refcounting and adoption. Locks are only needed if you intentionally build shared mutable structures outside this model.

### Do per‑VM GCs need to stop‑the‑world for shared regions?

It depends on what “shared” means. Our design keeps per‑VM GCs independent by default:

1) Untraced shared regions (recommended)
   - Contents contain no GC pointers; GCs never scan them.
   - Each VM can collect independently; no cross‑VM safepoints or barriers are required.
   - Lifetime is managed explicitly (refcounts/finalizers/epochs). This is how `fun_shared_buffer` works.

2) Shared, GC‑managed objects (generally avoid)
   - If objects with pointers are genuinely shared across VMs, you need one of:
     - Global safepoints with coordinated stop‑the‑world across participants; or
     - A concurrent/incremental GC with cross‑VM read/write barriers and either a shared heap or remembered sets; or
     - Replacing tracing with atomic refcounting for the shared objects.
   - Complexity, latency, and correctness risks increase substantially. Fun intentionally avoids this mode.

3) Hybrids
   - Immutable shared objects with global refcounts and no pointers back into per‑VM heaps.
   - Copy‑on‑write pages or rope/cord structures backed by `fun_shared_buffer`.
   - Message passing that can “adopt” an off‑heap buffer by transferring ownership instead of copying.

### Practical guidance

- Default to isolated VMs and message passing.
- Use `fun_shared_buffer` only for zero‑copy payloads; keep them untraced and pointer‑free relative to VM heaps.
- Clearly document and enforce what types are shareable; forbid cross‑VM heap references.
- If you ever consider sharing GC‑managed objects, budget for global safepoints or a fully barriered concurrent GC — neither is planned for Fun’s core.

### Bottom line

- Isolates are the sensible default; shared‑memory primitives are the performance escape hatch.
- With off‑heap, untraced shared regions, per‑VM GCs stay independent and never have to stop the world across other VMs.

### Embedding patterns

- Parallelism: Create N VMs for N cores, wire them with channels or shared buffers. No global lock contention.
- UI/game loop: Keep one VM on the main thread; background workers operate their own VMs and communicate via ports.
- Native callbacks/FFI: Callbacks into a VM must occur on its owning thread (use `fun_vm_post`). For bulk data (e.g., images, tensors), pass `fun_shared_buffer` to avoid copies.

### Why not a GIL?

- A GIL simplifies internal invariants but serializes all CPU‑bound work and penalizes embedded hosts with existing thread pools. Our isolate + message‑passing design preserves safety while scaling with cores.

### When you must share state

- Prefer `fun_shared_buffer` (immutable) or message passing.
- If you truly need shared mutability from native code, use `fun_atomic_*` or `fun_mutex_t`/`fun_rwlock_t` around your own data structures outside the VM. The VM treats these as external resources.

### Minimal API surface (illustrative)

```c
// Create/destroy VMs
fun_vm_t* vm = fun_vm_create(const fun_vm_config_t*);
void fun_vm_destroy(fun_vm_t*);

// Thread-affine execution and posting work
int fun_vm_run(fun_vm_t*, const fun_script_t*);
int fun_vm_post(fun_vm_t*, void (*cb)(fun_vm_t*, void*), void* user);

// Ports/channels for inter-VM comms
fun_port_t* fun_port_create(fun_vm_t*);
int fun_send(fun_port_t*, fun_value_t value);
int fun_recv(fun_port_t*, fun_value_t* out, uint64_t timeout_ms);

// Serialization for cross-VM values
int fun_serialize(fun_value_t v, fun_buffer_t* out);
int fun_deserialize(fun_vm_t*, const fun_buffer_t*, fun_value_t* out);

// Zero-copy shared buffers (immutable inside VMs)
fun_shared_buffer_t* fun_shared_buffer_new(size_t n);
void* fun_shared_buffer_data(fun_shared_buffer_t*);
void fun_shared_buffer_retain(fun_shared_buffer_t*);
void fun_shared_buffer_release(fun_shared_buffer_t*);
```

### Glossary

- GC: garbage collection. In our context, each `fun_vm_t` isolate has its own GC (stop‑the‑world, per‑VM). There is no global stop‑the‑world and no global lock; a GC pause in one VM does not affect others.
