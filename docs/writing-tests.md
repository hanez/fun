# Writing Tests

How to author tests for Fun and its opcode implementations.

## Test kinds
- VM/opcode tests: exercise bytecode operations and VM behavior (target: `test_opcodes`).
- Language/runtime tests: exercise user-visible features and stdlib behavior (target: `fun_test`).

See `docs/testing.md` for how to build and run these targets with CTest.

## Adding new tests
The exact layout may evolve; generally:
- Add new test sources alongside existing ones used by `fun_test`.
- For opcode-focused tests, add cases where `test_opcodes` discovers them.

Aim for:
- Small, isolated test cases with clear assertions
- One behavior per test; descriptive names
- Minimal fixtures and setup

## Guidelines
- Prefer black-box testing via public APIs.
- When adding features, include both positive and negative cases.
- Keep tests deterministic; avoid non-deterministic timers or random sources unless seeded.
