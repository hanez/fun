cmake_minimum_required(VERSION 3.16)
project(fun VERSION 0.17.8 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
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
# Ensure trailing slash
if(NOT DEFAULT_LIB_DIR MATCHES "/$")
  set(DEFAULT_LIB_DIR "${DEFAULT_LIB_DIR}/")
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
    target_compile_definitions(fun_core PUBLIC FUN_DEBUG=1)
endif()

# Provide default stdlib directory to the runtime
target_compile_definitions(fun_core PUBLIC DEFAULT_LIB_DIR="${DEFAULT_LIB_DIR}")

# PCSC include and link (if enabled)
if(PCSC_INCLUDE_DIRS)
  target_include_directories(fun_core PRIVATE ${PCSC_INCLUDE_DIRS})
endif()
if(PCSC_LINK_LIBS)
  target_link_libraries(fun_core PUBLIC ${PCSC_LINK_LIBS})
endif()

# Link threads if available on UNIX
if(Threads_FOUND)
  target_link_libraries(fun_core PUBLIC Threads::Threads)
endif()

# Interpreter /usr/bin/fun
option(FUN_WITH_REPL "Enable interactive REPL in the fun CLI" OFF)
add_executable(fun
        src/fun.c
)
# Provide version string to the CLI
target_compile_definitions(fun PRIVATE FUN_VERSION=\"${PROJECT_VERSION}\")
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
  COMMAND ${CMAKE_COMMAND} -E env FUN_LIB_DIR="${FUN_LIB}" ${CMAKE_SOURCE_DIR}/scripts/run_examples.sh
  DEPENDS fun
  USES_TERMINAL
  COMMENT "Build and run all examples"
)

add_custom_target(run-examples
  COMMAND ${CMAKE_COMMAND} -E env FUN_LIB_DIR="${FUN_LIB}" ${CMAKE_SOURCE_DIR}/scripts/run_examples.sh
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
        RUNTIME DESTINATION /usr/bin)

# Libs
install(DIRECTORY lib/
        DESTINATION /usr/share/fun/lib
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
