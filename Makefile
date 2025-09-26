# Simple Makefile wrapper around CMake

BUILD_DIR ?= build
CMAKE ?= cmake
# Default to a build with extra logging disabled
CMAKE_FLAGS ?= -DFUN_DEBUG=ON

.PHONY: all configure build fun fun_test test_opcodes clean distclean install repl run

all: clean fun fun_test test_opcodes

# One-time configure (re-runs harmlessly)
configure:
	$(CMAKE) -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)

# Build aggregate
build: configure
	$(CMAKE) --build $(BUILD_DIR)

fun: configure
	$(CMAKE) --build $(BUILD_DIR) --target fun

fun_test: configure
	$(CMAKE) --build $(BUILD_DIR) --target fun_test

test_opcodes: configure
	$(CMAKE) --build $(BUILD_DIR) --target test_opcodes

# Clean via CMake (if configured)
clean:
	$(CMAKE) --build $(BUILD_DIR) --target clean || true

# Full clean: remove the build directory entirely
distclean:
	$(CMAKE) --build $(BUILD_DIR) --target distclean || true

# Install to fixed paths defined in CMake (may require sudo):
# - Binary   : /usr/bin/fun
# - Examples : /usr/share/fun/examples/*.fun
# - Docs     : /usr/share/doc/fun/README.md, LICENSE
install:
	$(CMAKE) --install $(BUILD_DIR)

# Convenience: run REPL
repl: fun
	./$(BUILD_DIR)/fun

# Convenience: run a script (usage: make run SCRIPT=examples/strings_test.fun)
run: fun
	./$(BUILD_DIR)/fun $(SCRIPT)

# Additional convenience targets
.PHONY: verify-ops examples run-examples

# Verify that each OP_* in bytecode.h has a corresponding vm_case_*.inc include in vm.c
verify-ops:
	@./scripts/check_op_includes.py --verbose

# Build and run all examples (searches for the fun binary automatically)
examples run-examples: fun
	@./scripts/run_examples.sh
