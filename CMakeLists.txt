cmake_minimum_required(VERSION 3.16)
project(fun C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Core VM/library sources
add_library(fun_core
        src/bytecode.c
        src/value.c
        src/vm.c
)

target_include_directories(fun_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Playground / interpreter (future /usr/bin/fun)
add_executable(fun
        src/fun.c
        examples/hello_world.fun
)
target_link_libraries(fun PRIVATE fun_core)

# Internal test programs
add_executable(fun_test 
        src/fun_test.c
)
target_link_libraries(fun_test PRIVATE fun_core)

add_executable(test_opcodes 
        src/test_opcodes.c
)
target_link_libraries(test_opcodes PRIVATE fun_core)

# Convenience targets for cleaning (optional)
add_custom_target(clean-build
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMENT "Clean build outputs (objects, binaries) in the build directory"
)
add_custom_target(dist-clean
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
        COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR}/CMakeCache.txt
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_BINARY_DIR}/cmake_install.cmake
        ${CMAKE_BINARY_DIR}/install_manifest.txt
        ${CMAKE_BINARY_DIR}/Makefile
        ${CMAKE_BINARY_DIR}/*.ninja
        ${CMAKE_BINARY_DIR}/.ninja_*
        COMMENT "Remove build outputs and CMake-generated files (reconfigure needed)"
)
