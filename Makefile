all:
	make clean
	make fun
	make fun_test
	make test_opcodes

fun:
	cmake -S . -B build -DFUN_DEBUG=ON
	cmake --build build --target fun

fun_test:
	cmake -S . -B build -DFUN_DEBUG=ON
	cmake --build build --target fun_test

clean:
	cmake --build build --target clean-build

# Or set a custom prefix: cmake --install build --prefix "$HOME/.local"
# Defaults:
# Binary: /usr/bin/fun
# Docs: /share/doc/fun/README.md, LICENSE
# Examples: /share/fun/examples/*.fun
install:
	cmake --install build

test_opcodes:
	cmake -S . -B build -DFUN_DEBUG=ON
	cmake --build build --target test_opcodes
