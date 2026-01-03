cmake_minimum_required(VERSION 3.10)
project(fun VERSION 0.37.42 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# Defaults and options migrated from Makefile
# Convenience: default path to bundled stdlib
set(FUN_LIB "${CMAKE_SOURCE_DIR}/lib" CACHE PATH "Path to bundled Fun stdlib")

# Optional: override default library directory for #include <...> lookups
# -DDEFAULT_LIB_DIR=/custom/fun/lib (trailing slash added automatically)
if(NOT DEFINED DEFAULT_LIB_DIR OR DEFAULT_LIB_DIR STREQUAL "")
  if(WIN32)
    set(_FUN_DEFAULT_LIB_DIR "C:/Users/Public/fun/lib")
  elseif(APPLE)
    set(_FUN_DEFAULT_LIB_DIR "/Library/Application Support/fun/lib")
  else()
    set(_FUN_DEFAULT_LIB_DIR "/usr/share/fun/lib")
  endif()
    set(DEFAULT_LIB_DIR "${_FUN_DEFAULT_LIB_DIR}" CACHE PATH "Default library directory for Fun stdlib (override with -DDEFAULT_LIB_DIR=...)" FORCE)
else()
  set(DEFAULT_LIB_DIR "${DEFAULT_LIB_DIR}" CACHE PATH "Default library directory for Fun stdlib (override with -DDEFAULT_LIB_DIR=...)" FORCE)
endif()

# Optional: build using musl libc instead of glibc on Linux (OFF by default)
# Note: Switching CMAKE_C_COMPILER must happen before the first project() call.
option(FUN_USE_MUSL "Use musl libc toolchain when available (Linux only)" OFF)

if(FUN_USE_MUSL AND UNIX AND NOT APPLE)
  # Try to find a musl toolchain wrapper
  find_program(_FUN_MUSL_CC NAMES musl-gcc musl-clang)
  if(_FUN_MUSL_CC)
    message(STATUS "FUN_USE_MUSL=ON: using musl toolchain: ${_FUN_MUSL_CC}")
    # Force C compiler to musl wrapper before project() so the whole toolchain is configured accordingly
    set(CMAKE_C_COMPILER "${_FUN_MUSL_CC}" CACHE FILEPATH "C compiler" FORCE)
    set(FUN_LIBC "musl" CACHE STRING "Selected C library")
  else()
    message(WARNING "FUN_USE_MUSL=ON but no musl toolchain (musl-gcc or musl-clang) found. Falling back to default compiler (likely glibc).")
    set(FUN_LIBC "glibc" CACHE STRING "Selected C library")
  endif()
else()
  # Default remains the system toolchain (typically glibc on Linux)
  set(FUN_LIBC "glibc" CACHE STRING "Selected C library")
endif()

# Expose a preprocessor macro indicating selected C library
if(FUN_LIBC STREQUAL "musl")
  add_definitions(-DFUN_LIBC_MUSL)
else()
  add_definitions(-DFUN_LIBC_GLIBC)
endif()

# Optional: build statically linked executables
# When enabled, prefer static libraries for all dependencies and request
# fully static linking for executables where the platform/toolchain allows it.
option(FUN_LINK_STATIC "Link fun and tests fully static where possible" OFF)

if(FUN_LINK_STATIC)
  message(STATUS "Building Fun statically linked")
  # Prefer static libraries for all add_library without explicit type
  set(BUILD_SHARED_LIBS OFF)

  # Make pkg-config choose static libs
  set(PKG_CONFIG_USE_STATIC_LIBS ON)

  # Hint find_library to choose static archives first on Unix (not macOS)
  # Prefer .a but still allow falling back to shared if static is unavailable.
  if(UNIX AND NOT APPLE)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a;.so;.so.0;.so.1)
  endif()

  # Toolchain-specific static link flags
  # Use a "mostly static" approach by default to avoid requiring static
  # variants of every system/third-party library. This keeps libgcc and
  # libstdc++ static while allowing shared deps when needed.
  if(UNIX AND NOT APPLE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(_FUN_STATIC_LINK_FLAGS -static-libgcc -static-libstdc++)
  endif()

  # On MSVC, prefer the static runtime
  if(MSVC)
    foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE 
            CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL)
      if(DEFINED ${flag_var})
        string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif()
    endforeach()
  endif()
endif()

# Optional Tcl/Tk GUI support (embedded interpreter)
option(FUN_WITH_TCLTK "Enable Tcl/Tk GUI support" OFF)
set(TCL_INCLUDE_DIRS "")
set(TCL_LINK_LIBS "")
if(FUN_WITH_TCLTK)
  message(STATUS "Building with Tcl/Tk support")
  add_definitions(-DFUN_WITH_TCLTK)
  # Try pkg-config first
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(TCL QUIET tcl)
    pkg_check_modules(TK  QUIET tk)
  endif()
  if(TCL_FOUND OR TK_FOUND)
    if(TCL_FOUND)
      list(APPEND TCL_INCLUDE_DIRS ${TCL_INCLUDE_DIRS} ${TCL_INCLUDE_DIRS})
      list(APPEND TCL_LINK_LIBS ${TCL_LINK_LIBS} ${TCL_LIBRARIES})
    endif()
    if(TK_FOUND)
      list(APPEND TCL_INCLUDE_DIRS ${TCL_INCLUDE_DIRS} ${TK_INCLUDE_DIRS})
      list(APPEND TCL_LINK_LIBS ${TCL_LINK_LIBS} ${TK_LIBRARIES})
    endif()
    if(TCL_INCLUDE_DIRS)
      include_directories(${TCL_INCLUDE_DIRS})
    endif()
  else()
    # Fallbacks by platform (best-effort)
    find_path(TCL_INCLUDE_DIR tcl.h PATH_SUFFIXES tcl8.7 tcl8.6 include)
    find_library(TCL_LIB NAMES tcl8.7 tcl8.6 tcl)
    find_library(TK_LIB  NAMES tk8.7  tk8.6  tk)
    if(TCL_INCLUDE_DIR)
      list(APPEND TCL_INCLUDE_DIRS ${TCL_INCLUDE_DIR})
      include_directories(${TCL_INCLUDE_DIR})
    endif()
    if(TCL_LIB)
      list(APPEND TCL_LINK_LIBS ${TCL_LIB})
    endif()
    if(TK_LIB)
      list(APPEND TCL_LINK_LIBS ${TK_LIB})
    endif()
    if(APPLE)
      # On macOS additional frameworks are usually not required; Homebrew libs suffice
    elseif(WIN32)
      # Typical Windows GUI libs
      list(APPEND TCL_LINK_LIBS user32 gdi32 comctl32)
    else()
      # X11 may be required on some Linux setups
      find_package(X11 QUIET)
      if(X11_FOUND)
        include_directories(${X11_INCLUDE_DIR})
        list(APPEND TCL_LINK_LIBS ${X11_LIBRARIES})
      endif()
    endif()
    if(NOT TCL_LINK_LIBS)
      message(WARNING "Tcl/Tk libraries not found via pkg-config or fallbacks. FUN_WITH_TCLTK is enabled, but linking may fail.")
    endif()
  endif()
endif()
# Ensure trailing slash
if(NOT DEFAULT_LIB_DIR MATCHES "/$")
  set(DEFAULT_LIB_DIR "${DEFAULT_LIB_DIR}/")
endif()

# Optional SQLite support
option(FUN_WITH_SQLITE "Enable SQLite (sqlite3) support" OFF)
set(SQLITE3_INCLUDE_DIRS "")
set(SQLITE3_LINK_LIBS "")
if(FUN_WITH_SQLITE)
  message(STATUS "Building with SQLite support")
  add_definitions(-DFUN_WITH_SQLITE)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SQLITE3 QUIET sqlite3)
  endif()
  if(SQLITE3_FOUND)
    list(APPEND SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
    list(APPEND SQLITE3_LINK_LIBS ${SQLITE3_LINK_LIBS} ${SQLITE3_LIBRARIES})
    include_directories(${SQLITE3_INCLUDE_DIRS})
  else()
    find_library(SQLITE3_LIB sqlite3)
    if(SQLITE3_LIB)
      list(APPEND SQLITE3_LINK_LIBS ${SQLITE3_LIB})
    else()
      message(FATAL_ERROR "sqlite3 not found. Install sqlite3 (dev headers) or disable FUN_WITH_SQLITE.")
    endif()
  endif()
endif()

# Optional PCSC support (enabled via -DFUN_WITH_PCSC=ON)
option(FUN_WITH_PCSC "Enable PCSC (pcsclite) support" OFF)
set(PCSC_LINK_LIBS "")
set(PCSC_INCLUDE_DIRS "")
if(FUN_WITH_PCSC)
  message(STATUS "Building with PCSC support")
  add_definitions(-DFUN_WITH_PCSC)
  if(APPLE)
    list(APPEND PCSC_LINK_LIBS "-framework PCSC")
  else()
    list(APPEND PCSC_INCLUDE_DIRS "/usr/include/PCSC")
    list(APPEND PCSC_LINK_LIBS pcsclite)
  endif()
endif()

# Optional JSON (json-c) support
option(FUN_WITH_JSON "Enable JSON (json-c) support" OFF)
set(JSONC_INCLUDE_DIRS "")
set(JSONC_LINK_LIBS "")
if(FUN_WITH_JSON)
  message(STATUS "Building with JSON (json-c) support")
  add_definitions(-DFUN_WITH_JSON)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(JSONC QUIET json-c)
  endif()
  if(JSONC_FOUND)
    list(APPEND JSONC_INCLUDE_DIRS ${JSONC_INCLUDE_DIRS} ${JSONC_INCLUDE_DIRS})
    list(APPEND JSONC_LINK_LIBS ${JSONC_LINK_LIBS} ${JSONC_LIBRARIES})
    include_directories(${JSONC_INCLUDE_DIRS})
  else()
    # Fallback: try plain -ljson-c
    list(APPEND JSONC_LINK_LIBS json-c)
  endif()
endif()

# Optional INI (iniparser 4.2.6) support
option(FUN_WITH_INI "Enable INI (iniparser) support" OFF)
set(INIPARSER_INCLUDE_DIRS "")
set(INIPARSER_LINK_LIBS "")
if(FUN_WITH_INI)
  message(STATUS "Building with INI (iniparser) support")
  add_definitions(-DFUN_WITH_INI)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(INIPARSER QUIET iniparser)
  endif()
  if(INIPARSER_FOUND)
    list(APPEND INIPARSER_INCLUDE_DIRS ${INIPARSER_INCLUDE_DIRS} ${INIPARSER_INCLUDE_DIRS})
    list(APPEND INIPARSER_LINK_LIBS ${INIPARSER_LINK_LIBS} ${INIPARSER_LIBRARIES})
  else()
    # Fallback: try to locate headers and library manually
    find_path(INIPARSER_INCLUDE_DIR iniparser.h)
    find_library(INIPARSER_LIB NAMES iniparser)
    if(INIPARSER_INCLUDE_DIR)
      list(APPEND INIPARSER_INCLUDE_DIRS ${INIPARSER_INCLUDE_DIR})
    endif()
    if(INIPARSER_LIB)
      list(APPEND INIPARSER_LINK_LIBS ${INIPARSER_LIB})
    endif()
    if(NOT INIPARSER_INCLUDE_DIRS OR NOT INIPARSER_LINK_LIBS)
      message(FATAL_ERROR "iniparser not found. Install iniparser (>=4.2.6) or disable FUN_WITH_INI.")
    endif()
  endif()
endif()

# Optional XML (libxml2) support
option(FUN_WITH_XML2 "Enable XML (libxml2) support" OFF)
set(LIBXML2_INCLUDE_DIRS "")
set(LIBXML2_LINK_LIBS "")
if(FUN_WITH_XML2)
  message(STATUS "Building with XML (libxml2) support")
  add_definitions(-DFUN_WITH_XML2)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBXML2 QUIET libxml-2.0)
  endif()
  if(LIBXML2_FOUND)
    list(APPEND LIBXML2_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
    list(APPEND LIBXML2_LINK_LIBS ${LIBXML2_LINK_LIBS} ${LIBXML2_LIBRARIES})
    include_directories(${LIBXML2_INCLUDE_DIRS})
  else()
    find_library(LIBXML2_LIB NAMES xml2 libxml2)
    if(LIBXML2_LIB)
      list(APPEND LIBXML2_LINK_LIBS ${LIBXML2_LIB})
      # Common system include location for libxml2 headers
      if(EXISTS "/usr/include/libxml2")
        list(APPEND LIBXML2_INCLUDE_DIRS "/usr/include/libxml2")
        include_directories(${LIBXML2_INCLUDE_DIRS})
      endif()
  else()
    message(FATAL_ERROR "libxml2 not found. Install libxml2 (dev headers) or disable FUN_WITH_XML2.")
  endif()
  endif()
  # As a robust fallback, add standard system include path for libxml2 if present
  if(EXISTS "/usr/include/libxml2")
    include_directories("/usr/include/libxml2")
  endif()
endif()

# Optional libsql support (independent from SQLite)
option(FUN_WITH_LIBSQL "Enable libsql (Turso) client support" OFF)
set(LIBSQL_INCLUDE_DIRS "")
set(LIBSQL_LINK_LIBS "")
if(FUN_WITH_LIBSQL)
  message(STATUS "Building with libSQL support")
  add_definitions(-DFUN_WITH_LIBSQL)
  # Try pkg-config for libsql first; some systems expose libsql-client
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBSQL QUIET libsql)
    if(NOT LIBSQL_FOUND)
      pkg_check_modules(LIBSQL_CLIENT QUIET libsql-client)
      if(LIBSQL_CLIENT_FOUND)
        set(LIBSQL_FOUND TRUE)
        set(LIBSQL_INCLUDE_DIRS ${LIBSQL_CLIENT_INCLUDE_DIRS})
        set(LIBSQL_LINK_LIBS ${LIBSQL_CLIENT_LIBRARIES})
      endif()
    endif()
  endif()
  if(LIBSQL_FOUND)
    include_directories(${LIBSQL_INCLUDE_DIRS})
  else()
    # Fallback: many libsql deployments provide a sqlite3-compatible client lib
    # so try linking against sqlite3 as a compatibility layer.
    find_library(LIBSQL_LIB sqlite3)
    if(LIBSQL_LIB)
      list(APPEND LIBSQL_LINK_LIBS ${LIBSQL_LIB})
    else()
      message(FATAL_ERROR "libsql not found. Install libsql (or compatible sqlite3 client) or disable FUN_WITH_LIBSQL.")
    endif()
  endif()
endif()

# Optional PCRE2 support
option(FUN_WITH_PCRE2 "Enable PCRE2 (Perl Compatible Regular Expressions) support" OFF)
set(PCRE2_INCLUDE_DIRS "")
set(PCRE2_LINK_LIBS "")
if(FUN_WITH_PCRE2)
  message(STATUS "Building with PCRE2 support")
  add_definitions(-DFUN_WITH_PCRE2)
  # Try pkg-config first
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PCRE2 QUIET libpcre2-8)
  endif()
  if(PCRE2_FOUND)
    list(APPEND PCRE2_INCLUDE_DIRS ${PCRE2_INCLUDE_DIRS} ${PCRE2_INCLUDE_DIRS})
    list(APPEND PCRE2_LINK_LIBS ${PCRE2_LINK_LIBS} ${PCRE2_LIBRARIES})
    include_directories(${PCRE2_INCLUDE_DIRS})
  else()
    # Fallback: common defaults
    list(APPEND PCRE2_LINK_LIBS pcre2-8)
  endif()
endif()

# Optional CURL (libcurl) support
option(FUN_WITH_CURL "Enable libcurl HTTP client support" OFF)
set(CURL_INCLUDE_DIRS "")
set(CURL_LINK_LIBS "")
if(FUN_WITH_CURL)
  message(STATUS "Building with libcurl support")
  add_definitions(-DFUN_WITH_CURL)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBCURL QUIET libcurl)
  endif()
  if(LIBCURL_FOUND)
    list(APPEND CURL_INCLUDE_DIRS ${LIBCURL_INCLUDE_DIRS})
    list(APPEND CURL_LINK_LIBS ${LIBCURL_LIBRARIES})
    include_directories(${CURL_INCLUDE_DIRS})
  else()
    list(APPEND CURL_LINK_LIBS curl)
  endif()
endif()

# Debug option to enable verbose parser/VM logging
option(FUN_DEBUG "Enable extra debug logging in Fun" OFF)

# Threads (used by demos and optionally by the runtime)
if(UNIX)
  find_package(Threads QUIET)
endif()

# Core VM/library sources
add_library(fun_core
  src/bytecode.c
  src/parser.c
  src/value.c
  src/vm.c
)

target_include_directories(fun_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Apply options to core
if(FUN_DEBUG)
  message(STATUS "FUN_DEBUG enabled: building with verbose debug logging")
  target_compile_definitions(fun_core PUBLIC FUN_VERSION="${PROJECT_VERSION}")
  target_compile_definitions(fun_core PUBLIC FUN_DEBUG=1)
endif()

# Provide default stdlib directory to the runtime
target_compile_definitions(fun_core PUBLIC FUN_VERSION="${PROJECT_VERSION}")
target_compile_definitions(fun_core PUBLIC DEFAULT_LIB_DIR="${DEFAULT_LIB_DIR}")

# PCSC include and link (if enabled)
if(PCSC_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${PCSC_INCLUDE_DIRS})
endif()
if(PCSC_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${PCSC_LINK_LIBS})
endif()

# json-c include and link (if enabled)
if(JSONC_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${JSONC_INCLUDE_DIRS})
endif()
if(JSONC_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${JSONC_LINK_LIBS})
endif()

# pcre2 include and link (if enabled)
if(PCRE2_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${PCRE2_INCLUDE_DIRS})
endif()
if(PCRE2_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${PCRE2_LINK_LIBS})
endif()

# libcurl include and link (if enabled)
if(CURL_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${CURL_INCLUDE_DIRS})
endif()
if(CURL_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${CURL_LINK_LIBS})
endif()

# sqlite3 include and link (if enabled)
if(SQLITE3_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${SQLITE3_INCLUDE_DIRS})
endif()
if(SQLITE3_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${SQLITE3_LINK_LIBS})
endif()

# iniparser include and link (if enabled)
if(INIPARSER_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${INIPARSER_INCLUDE_DIRS})
endif()
if(INIPARSER_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${INIPARSER_LINK_LIBS})
endif()

# libsql include and link (if enabled)
if(LIBSQL_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${LIBSQL_INCLUDE_DIRS})
endif()
if(LIBSQL_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${LIBSQL_LINK_LIBS})
endif()

# libxml2 include and link (if enabled)
if(LIBXML2_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${LIBXML2_INCLUDE_DIRS})
endif()
if(LIBXML2_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${LIBXML2_LINK_LIBS})
endif()
if(FUN_WITH_XML2)
  if(EXISTS "/usr/include/libxml2")
    target_include_directories(fun_core PRIVATE "/usr/include/libxml2")
  endif()
endif()

# Tcl/Tk include and link (if enabled)
if(TCL_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${TCL_INCLUDE_DIRS})
endif()
if(TCL_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${TCL_LINK_LIBS})
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

# Interpreter /usr/bin/fun
option(FUN_WITH_REPL "Enable interactive REPL in the fun CLI" OFF)
add_executable(fun
  src/fun.c
)
# Provide version string to the CLI
if(FUN_WITH_REPL)
  # Enable REPL compilation path and add the REPL source
  target_compile_definitions(fun PRIVATE FUN_WITH_REPL=1)
  target_sources(fun PRIVATE src/repl.c)
endif()
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

# If static linking is requested, apply linker flags to executables
if(FUN_LINK_STATIC AND DEFINED _FUN_STATIC_LINK_FLAGS)
  foreach(tgt fun fun_test test_opcodes)
    if(TARGET ${tgt})
      target_link_options(${tgt} PRIVATE ${_FUN_STATIC_LINK_FLAGS})
    endif()
  endforeach()
endif()

# Convenience aggregate target (like 'build' in Makefile)
add_custom_target(build
  DEPENDS fun fun_test test_opcodes
)

# Convenience targets: repl, run, threads-demo, ops, examples
set(FUN_RUN_SCRIPT "" CACHE STRING "Script to run with the 'run' target, e.g. -DFUN_RUN_SCRIPT=examples/strings_test.fun")

if(FUN_WITH_REPL)
  add_custom_target(repl
  COMMAND $<TARGET_FILE:fun>
  DEPENDS fun
  USES_TERMINAL
  COMMENT "Run Fun REPL"
)
endif()

add_custom_target(run
  COMMAND $<TARGET_FILE:fun> ${FUN_RUN_SCRIPT}
  DEPENDS fun
  USES_TERMINAL
  COMMENT "Run Fun with script: ${FUN_RUN_SCRIPT}"
)

add_custom_target(threads-demo
  COMMAND ${CMAKE_COMMAND} -E echo "Running Thread class demo with FUN_LIB_DIR=${FUN_LIB}"
  COMMAND ${CMAKE_COMMAND} -E env FUN_LIB_DIR="${FUN_LIB}" $<TARGET_FILE:fun> ${CMAKE_SOURCE_DIR}/examples/threads_demo.fun
  DEPENDS fun
  USES_TERMINAL
)

# Python for helper scripts
find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  set(_FUN_PY "${Python3_EXECUTABLE}")
else()
  set(_FUN_PY "python3")
endif()

add_custom_target(ops
  COMMAND ${_FUN_PY} ${CMAKE_SOURCE_DIR}/scripts/check_op_includes.py --verbose
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  USES_TERMINAL
  COMMENT "Verbose opcode include check"
)

add_custom_target(ops-quiet
  COMMAND ${_FUN_PY} ${CMAKE_SOURCE_DIR}/scripts/check_op_includes.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  USES_TERMINAL
  COMMENT "Opcode include check"
)

add_custom_target(examples
  COMMAND ${CMAKE_COMMAND} -E env FUN_LIB_DIR="${FUN_LIB}" FUN_WITH_INI=$<IF:$<BOOL:${FUN_WITH_INI}>,1,0> ${CMAKE_SOURCE_DIR}/scripts/run_examples.sh
  DEPENDS fun
  USES_TERMINAL
  COMMENT "Build and run all examples"
)

add_custom_target(run-examples
  COMMAND ${CMAKE_COMMAND} -E env FUN_LIB_DIR="${FUN_LIB}" FUN_WITH_INI=$<IF:$<BOOL:${FUN_WITH_INI}>,1,0> ${CMAKE_SOURCE_DIR}/scripts/run_examples.sh
  DEPENDS fun
  USES_TERMINAL
  COMMENT "Build and run all examples"
)

# Clean and distclean
add_custom_target(fun_clean
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

# Robust uninstall support as a function (uses install_manifest*.txt)
function(fun_add_uninstall_target)
  set(_FUN_UNINSTALL_SCRIPT "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

  # Generate the cmake_uninstall.cmake script at configure time
  file(WRITE "${_FUN_UNINSTALL_SCRIPT}"
"set(_candidates \"\")
# Prefer explicit MANIFEST if provided
if(DEFINED MANIFEST AND NOT MANIFEST STREQUAL \"\")
  list(APPEND _candidates \"\${MANIFEST}\")
endif()

# Common locations (out-of-source and legacy in-source builds)
list(APPEND _candidates
  \"\${CMAKE_BINARY_DIR}/install_manifest.txt\"
  \"\${CMAKE_SOURCE_DIR}/install_manifest.txt\"
  \"\${CMAKE_BINARY_DIR}/install_manifest_Debug.txt\"
  \"\${CMAKE_BINARY_DIR}/install_manifest_Release.txt\"
  \"\${CMAKE_BINARY_DIR}/install_manifest_RelWithDebInfo.txt\"
  \"\${CMAKE_BINARY_DIR}/install_manifest_MinSizeRel.txt\"
  \"\${CMAKE_SOURCE_DIR}/install_manifest_Debug.txt\"
  \"\${CMAKE_SOURCE_DIR}/install_manifest_Release.txt\"
  \"\${CMAKE_SOURCE_DIR}/install_manifest_RelWithDebInfo.txt\"
  \"\${CMAKE_SOURCE_DIR}/install_manifest_MinSizeRel.txt\"
)

set(_manifest_file \"\")
foreach(_cand IN LISTS _candidates)
  if(EXISTS \"\${_cand}\")
  set(_manifest_file \"\${_cand}\")
  break()
  endif()
endforeach()

if(NOT _manifest_file)
  message(FATAL_ERROR
  \"Cannot find install manifest. Tried: \${_candidates}.
If you installed from a different build directory, run uninstall from that build directory (where install_manifest*.txt resides).\")
endif()

file(READ \"\${_manifest_file}\" _manifest)
string(REPLACE \"\\n\" \";\" _files \"\${_manifest}\")
set(_removed 0)
set(_dirs \"\")
foreach(_file IN LISTS _files)
  if(_file STREQUAL \"\")
  continue()
  endif()
  if(EXISTS \"\${_file}\" OR IS_SYMLINK \"\${_file}\")
  message(STATUS \"Uninstalling: \${_file}\")
  file(REMOVE \"\${_file}\")
  # If the file still exists after attempting to remove it, fail with an explicit error
  if(EXISTS \"\${_file}\" OR IS_SYMLINK \"\${_file}\")
    message(FATAL_ERROR \"Failed to remove: \${_file}. Try running the uninstall target with sudo if it is a system install.\")
  endif()
  # Track the containing directory for potential cleanup
  get_filename_component(_dir \"\${_file}\" DIRECTORY)
  list(APPEND _dirs \"\${_dir}\")
  math(EXPR _removed \"\${_removed}+1\")
  else()
  message(STATUS \"Skipping (not found): \${_file}\")
  endif()
endforeach()

# Remove empty directories that contained installed files (and their parents under safe roots)
# Safe roots we are allowed to prune if they become empty:
set(_safe_roots \"/usr/share/fun\" \"/usr/share/doc/fun\")

# Expand to include parent directories up to the safe roots
set(_all_dirs \"\")
foreach(_d IN LISTS _dirs)
  set(_p \"\${_d}\")
  while(NOT \"\${_p}\" STREQUAL \"\" AND NOT \"\${_p}\" STREQUAL \"/\")
  list(APPEND _all_dirs \"\${_p}\")
  get_filename_component(_p \"\${_p}\" DIRECTORY)
  # Stop collecting parents once we reach a safe root or the filesystem root
  list(FIND _safe_roots \"\${_p}\" _root_idx)
  if(_root_idx GREATER -1)
    list(APPEND _all_dirs \"\${_p}\")
    break()
  endif()
  endwhile()
endforeach()

# Deduplicate
list(REMOVE_DUPLICATES _all_dirs)

# Try multiple passes to remove parents that become empty after children are removed
set(_removed_dirs 0)
set(_pass 0)
while(TRUE)
  set(_pass_removed 0)
  foreach(_dir IN LISTS _all_dirs)
  if(EXISTS \"\${_dir}\" AND IS_DIRECTORY \"\${_dir}\")
    # Only act on directories within safe roots
    set(_allowed FALSE)
    foreach(_root IN LISTS _safe_roots)
    if(\"\${_dir}\" MATCHES \"^\${_root}(/|\$)\")
      set(_allowed TRUE)
      break()
    endif()
    endforeach()
    if(NOT _allowed)
    # e.g. /usr/bin — do not remove
    continue()
    endif()

    # Remove only if empty (do not recurse unrelated content)
    file(GLOB _dir_contents LIST_DIRECTORIES true \"\${_dir}/*\")
    list(LENGTH _dir_contents _dir_len)
    if(_dir_len EQUAL 0)
    message(STATUS \"Removing empty directory: \${_dir}\")
    file(REMOVE_RECURSE \"\${_dir}\")
    if(IS_DIRECTORY \"\${_dir}\")
      message(FATAL_ERROR \"Failed to remove directory: \${_dir}. Try running the uninstall target with sudo if it is a system install.\")
    endif()
    math(EXPR _removed_dirs \"\${_removed_dirs}+1\")
    math(EXPR _pass_removed \"\${_pass_removed}+1\")
    endif()
  endif()
  endforeach()

  math(EXPR _pass \"\${_pass}+1\")
  if(_pass_removed EQUAL 0 OR _pass GREATER 5)
  break()
  endif()
endwhile()

message(STATUS \"Uninstall finished. Removed \${_removed} files and \${_removed_dirs} directories (if empty) listed in \${_manifest_file}\")
")

  # Expose an 'uninstall' build target (run with sudo if system locations were used)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND}
      -D MANIFEST="${CMAKE_BINARY_DIR}/install_manifest.txt"
      -D CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
      -D CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
      -P "${_FUN_UNINSTALL_SCRIPT}"
    USES_TERMINAL
    COMMENT "Uninstall files installed by this project (use sudo if needed)"
)
endfunction()

# Define the uninstall target
fun_add_uninstall_target()

# Install rules
# Binary
install(TARGETS fun
  RUNTIME DESTINATION /usr/bin
)

# Libs
install(DIRECTORY lib/
  DESTINATION /usr/share/fun/lib
  FILES_MATCHING PATTERN "*.fun"
)

# Optionally install example scripts
option(FUN_INSTALL_EXAMPLES "Install example .fun scripts" ON)
if(FUN_INSTALL_EXAMPLES)
  install(DIRECTORY examples/ 
    DESTINATION /usr/share/fun/examples
    FILES_MATCHING PATTERN "*.fun"
  )
endif()

# - Docs
install(FILES README.md LICENSE
  DESTINATION /usr/share/doc/fun
)
