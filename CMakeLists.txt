cmake_minimum_required(VERSION 3.16)
project(fun VERSION 0.8.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Core VM/library sources
add_library(fun_core
        src/bytecode.c
        src/parser.c
        src/value.c
        src/vm.c
        src/parser.c
)

target_include_directories(fun_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Debug option to enable verbose parser/VM logging
option(FUN_DEBUG "Enable extra debug logging in Fun" OFF)
if(FUN_DEBUG)
    message(STATUS "FUN_DEBUG enabled: building with verbose debug logging")
    target_compile_definitions(fun_core PUBLIC FUN_DEBUG=1)
endif()

# Playground / interpreter (future /usr/bin/fun)
add_executable(fun
        src/fun.c
)
# Provide version string to the CLI
target_compile_definitions(fun PRIVATE FUN_VERSION=\"${PROJECT_VERSION}\")
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
add_custom_target(distclean
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

# Install rules
# Binary
install(TARGETS fun
        RUNTIME DESTINATION /usr/bin)

# Libs
install(DIRECTORY lib/
        DESTINATION /usr/lib/fun
        FILES_MATCHING PATTERN "*.fun")

# Optionally install example scripts
option(FUN_INSTALL_EXAMPLES "Install example .fun scripts" ON)
if(FUN_INSTALL_EXAMPLES)
    install(DIRECTORY examples/
            DESTINATION /usr/share/fun/examples
            FILES_MATCHING PATTERN "*.fun")
endif()

# - Docs
install(FILES README.md LICENSE
        DESTINATION /usr/share/doc/fun)
