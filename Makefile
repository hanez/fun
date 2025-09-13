# Compiler & flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Isrc
SRC = src/vm.c src/bytecode.c src/value.c
OBJ = $(SRC:.c=.o)

# Tests
TEST_SRC = src/test.c src/test_opcodes.c
TEST_BIN = $(TEST_SRC:src/%.c=%)

.PHONY: all test clean

# Default: build main demo
all: fun

fun: $(OBJ) src/fun.c
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
tests: $(OBJ) $(TEST_BIN)
	@for t in $(TEST_BIN); do \
		echo "Running $$t..."; \
		./$$t; \
		echo "--------------------------------"; \
	done

# Build each test executable
$(TEST_BIN): %: src/%.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $<

# Clean everything
clean:
	rm -f $(OBJ) fun $(TEST_BIN)

