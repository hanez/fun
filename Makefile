# Simple Makefile wrapper around CMake
BUILD_DIR ?= build
CMAKE ?= cmake
# Default to a build with extra logging disabled
CMAKE_FLAGS ?= -DFUN_DEBUG=OFF

# Convenience: default path to bundled stdlib
FUN_LIB ?= $(CURDIR)/lib

# Optional: override default library directory for #include <...> lookups
# Usage: make DEFAULT_LIB_DIR=/custom/fun/lib
# This injects a compiler define: -DDEFAULT_LIB_DIR="/custom/fun/lib/"
ifneq ($(strip $(DEFAULT_LIB_DIR)),)
CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS) -DDEFAULT_LIB_DIR=\\\"$(DEFAULT_LIB_DIR)/\\\""
CMAKE_FLAGS += -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS) -DDEFAULT_LIB_DIR=\\\"$(DEFAULT_LIB_DIR)/\\\""
else
# Provide OS-specific defaults for Windows, macOS, Linux/Unix
ifeq ($(OS),Windows_NT)
  CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS) -DDEFAULT_LIB_DIR=\\\"C:/Users/Public/fun/lib/\\\""
  CMAKE_FLAGS += -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS) -DDEFAULT_LIB_DIR=\\\"C:/Users/Public/fun/lib/\\\""
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS) -DDEFAULT_LIB_DIR=\\\"/Library/Application Support/fun/lib/\\\""
    CMAKE_FLAGS += -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS) -DDEFAULT_LIB_DIR=\\\"/Library/Application Support/fun/lib/\\\""
    CMAKE_FLAGS += -DCMAKE_EXE_LINKER_FLAGS="$(CMAKE_EXE_LINKER_FLAGS) -pthread"
  else
    CMAKE_FLAGS += -DCMAKE_C_FLAGS="$(CMAKE_C_FLAGS) -DDEFAULT_LIB_DIR=\\\"/usr/share/fun/lib/\\\""
    CMAKE_FLAGS += -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS) -DDEFAULT_LIB_DIR=\\\"/usr/share/fun/lib/\\\""
    CMAKE_FLAGS += -DCMAKE_EXE_LINKER_FLAGS="$(CMAKE_EXE_LINKER_FLAGS) -pthread"
  endif
endif
endif

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

# Run the OO threading demo with FUN_LIB_DIR set
threads-demo: fun
	@echo "Running Thread class demo with FUN_LIB_DIR=$(FUN_LIB)"
	FUN_LIB_DIR="$(FUN_LIB)" ./$(BUILD_DIR)/fun examples/threads_demo.fun

# Additional convenience targets
.PHONY: verify-ops examples run-examples threads-demo ops ops-quiet

# Shorthand aliases for opcode include check
ops:
	@./scripts/check_op_includes.py --verbose

ops-quiet:
	@./scripts/check_op_includes.py

# Build and run all examples (searches for the fun binary automatically)
examples run-examples: fun
	@FUN_LIB_DIR="$(FUN_LIB)" ./scripts/run_examples.sh
