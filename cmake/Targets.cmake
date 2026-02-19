# Targets for Fun project (moved from src/CMakeLists.txt)

# Core VM/library sources
add_library(fun_core
  ${CMAKE_SOURCE_DIR}/src/bytecode.c
  ${CMAKE_SOURCE_DIR}/src/parser.c
  ${CMAKE_SOURCE_DIR}/src/value.c
  ${CMAKE_SOURCE_DIR}/src/vm.c
)

# NOTE: Do not compile src/vm/*/*.c as independent units.
# Those files are meant to be included by src/vm.c inside a big switch.

target_include_directories(fun_core PUBLIC
  ${CMAKE_SOURCE_DIR}/src
)

# Apply options to core
if(FUN_DEBUG)
  target_compile_definitions(fun_core PUBLIC FUN_VERSION="${PROJECT_VERSION}")
  target_compile_definitions(fun_core PUBLIC FUN_DEBUG=1)
endif()

# Provide default stdlib directory and version to the runtime
target_compile_definitions(fun_core PUBLIC FUN_VERSION="${PROJECT_VERSION}")
target_compile_definitions(fun_core PUBLIC DEFAULT_LIB_DIR="${DEFAULT_LIB_DIR}")

# Apply extension include/link variables discovered in cmake/Extensions
foreach(var_pair
    PCSC
    JSONC
    PCRE2
    CURL
    SQLITE3
    INIPARSER
    LIBSQL
    LIBXML2
    TCL
    NOTCURSES
    OPENSSL)
  if(${var_pair}_INCLUDE_DIRS)
    target_include_directories(fun_core PRIVATE ${${var_pair}_INCLUDE_DIRS})
  endif()
  if(${var_pair}_LINK_LIBS)
    target_link_libraries(fun_core PUBLIC ${${var_pair}_LINK_LIBS})
  endif()
endforeach()

# Ensure feature compile definitions are applied to fun_core so
# conditional code blocks (#ifdef FUN_WITH_*) are compiled as expected.

# Feature-specific sources and compile definitions
if(FUN_WITH_PCSC)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_PCSC=1)
endif()

if(FUN_WITH_JSON)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_JSON=1)
endif()

if(FUN_WITH_INI)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_INI=1)
endif()

if(FUN_WITH_XML2)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_XML2=1)
endif()

if(FUN_WITH_LIBSQL)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_LIBSQL=1)
endif()

if(FUN_WITH_PCRE2)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_PCRE2=1)
endif()

if(FUN_WITH_CURL)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_CURL=1)
endif()

if(FUN_WITH_NOTCURSES)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_NOTCURSES=1)
endif()

if(FUN_WITH_SQLITE)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_SQLITE=1)
endif()

if(FUN_WITH_TCLTK)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_TCLTK=1)
endif()

if(FUN_WITH_OPENSSL)
  target_compile_definitions(fun_core PUBLIC FUN_WITH_OPENSSL=1)
endif()

# Workaround: provide a dummy rust_eh_personality to satisfy linker when using
# no_std Rust staticlib with panic abort (some toolchains still reference it).
# If needed, we could add a C shim here, but the Rust crate now defines
# rust_eh_personality itself, so no extra C sources are required.
if(FUN_WITH_RUST)
  if(TARGET fun)
    target_sources(fun PRIVATE ${CMAKE_SOURCE_DIR}/src/rust/eh_personality.c)
  endif()
endif()

# Special case for libxml2 system include path if enabled
if(FUN_WITH_XML2)
  if(EXISTS "/usr/include/libxml2")
    target_include_directories(fun_core PRIVATE "/usr/include/libxml2")
  endif()
endif()

# Link threads if available on UNIX
if(Threads_FOUND)
  target_link_libraries(fun_core PUBLIC Threads::Threads)
endif()

# Link libm for C99 math functions if available
find_library(M_LIB m)
if(M_LIB)
  target_link_libraries(fun_core PUBLIC ${M_LIB})
endif()

# Executable: fun (CLI)
add_executable(fun
  ${CMAKE_SOURCE_DIR}/src/fun.c
)
if(FUN_WITH_REPL)
  target_compile_definitions(fun PRIVATE FUN_WITH_REPL=1)
  target_sources(fun PRIVATE ${CMAKE_SOURCE_DIR}/src/repl.c)
endif()
target_link_libraries(fun PRIVATE fun_core)

# Internal test programs
add_executable(fun_test
  ${CMAKE_SOURCE_DIR}/src/fun_test.c)
target_link_libraries(fun_test PRIVATE fun_core)

add_executable(test_opcodes
  ${CMAKE_SOURCE_DIR}/src/test_opcodes.c)
target_link_libraries(test_opcodes PRIVATE fun_core)

# Static linking flags are intentionally not applied (deprecated behavior)
