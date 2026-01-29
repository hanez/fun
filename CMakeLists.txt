cmake_minimum_required(VERSION 3.10)
project(fun VERSION 0.38.5 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

include(${CMAKE_SOURCE_DIR}/cmake/Options.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/Extensions.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Targets.cmake)

# --- Always show key build toggles ---
# Normalize to ENABLED/DISABLED like the "Fun extension summary"
if(FUN_DEBUG)
  set(_FUN_DEBUG_STATE "ENABLED")
else()
  set(_FUN_DEBUG_STATE "DISABLED")
endif()

if(FUN_WITH_CPP)
    set(_FUN_WITH_CPP_STATE "ENABLED")
else()
    set(_FUN_WITH_CPP_STATE "DISABLED")
endif()

if(FUN_WITH_RUST)
    set(_FUN_WITH_RUST_STATE "ENABLED")
else()
    set(_FUN_WITH_RUST_STATE "DISABLED")
endif()

message(STATUS "==== Fun build options ====")
message(STATUS "  FUN_DEBUG: ${_FUN_DEBUG_STATE}")
message(STATUS "  FUN_WITH_CPP: ${_FUN_WITH_CPP_STATE}")
message(STATUS "  FUN_WITH_RUST: ${_FUN_WITH_RUST_STATE}")
message(STATUS "===========================")

# Convenience aggregate target (like 'build' in Makefile)
add_custom_target(build
  DEPENDS fun fun_test test_opcodes
)

# --- Rust (Cargo) integration: optionally build and link a staticlib with opcode examples ---
option(FUN_WITH_RUST "Build and link Rust opcode library" OFF)

if(FUN_WITH_RUST)
  find_program(CARGO_EXECUTABLE cargo)
  if(NOT CARGO_EXECUTABLE)
    message(FATAL_ERROR "FUN_WITH_RUST=ON but 'cargo' not found in PATH")
  endif()

  set(RUST_CRATE_DIR ${CMAKE_SOURCE_DIR}/src/rust)

  # Map CMake build type to Cargo profile and output path
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RUST_PROFILE "debug")
    set(RUST_BUILD_ARGS)
  else()
    set(RUST_PROFILE "release")
    set(RUST_BUILD_ARGS --release)
  endif()

  # Crate name as per Cargo.toml
  # Crate name on-disk replaces '-' with '_' in artifact names
  set(RUST_CRATE_NAME hello-c-world)
  string(REPLACE "-" "_" RUST_CRATE_BASENAME "${RUST_CRATE_NAME}")
  set(RUST_LIB_NAME lib${RUST_CRATE_BASENAME}.a)
  set(RUST_LIB_PATH ${RUST_CRATE_DIR}/target/${RUST_PROFILE}/${RUST_LIB_NAME})

  add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND ${CARGO_EXECUTABLE} build ${RUST_BUILD_ARGS}
    WORKING_DIRECTORY ${RUST_CRATE_DIR}
    COMMENT "Building Rust static library (${RUST_PROFILE})"
    VERBATIM
  )

  add_custom_target(rust_ops_build DEPENDS ${RUST_LIB_PATH})

  add_library(fun_ops STATIC IMPORTED GLOBAL)
  set_target_properties(fun_ops PROPERTIES
    IMPORTED_LOCATION ${RUST_LIB_PATH}
    IMPORTED_LINK_INTERFACE_LANGUAGES C
  )
  add_dependencies(fun_ops rust_ops_build)

  # Link Rust ops into the core so executables can call them
  if(TARGET fun_core)
    add_dependencies(fun_core rust_ops_build)
    target_link_libraries(fun_core PRIVATE fun_ops)
    target_compile_definitions(fun_core PRIVATE FUN_WITH_RUST)
  endif()

  # Propagate define to test targets that might call Rust
  if(TARGET test_opcodes)
    add_dependencies(test_opcodes rust_ops_build)
    target_link_libraries(test_opcodes PRIVATE fun_ops)
    target_compile_definitions(test_opcodes PRIVATE FUN_WITH_RUST)
  endif()

  if(TARGET fun)
    target_compile_definitions(fun PRIVATE FUN_WITH_RUST)
  endif()
endif()

# --- Optional C++ opcode demo ---
option(FUN_WITH_CPP "Build and link C++ opcode demo" OFF)
if(FUN_WITH_CPP)
  enable_language(CXX)

  add_library(fun_cpp_ops STATIC
    src/vm/cpp/add.cpp
  )

  # Compile definitions propagate to C targets that dispatch the opcode
  target_compile_definitions(fun_cpp_ops PUBLIC FUN_WITH_CPP)
  target_include_directories(fun_cpp_ops PUBLIC ${CMAKE_SOURCE_DIR}/src)

  # Link the C++ ops into the core so executables can call them
  if(TARGET fun_core)
    target_link_libraries(fun_core PRIVATE fun_cpp_ops)
    target_compile_definitions(fun_core PRIVATE FUN_WITH_CPP)
  endif()

  # Also propagate to test targets (if they might use it)
  if(TARGET test_opcodes)
    target_link_libraries(test_opcodes PRIVATE fun_cpp_ops)
    target_compile_definitions(test_opcodes PRIVATE FUN_WITH_CPP)
  endif()

  if(TARGET fun)
    target_compile_definitions(fun PRIVATE FUN_WITH_CPP)
  endif()
endif()

# --- Size optimization for final binaries (Release) ---
# Enable function/data sectioning for better GC; safe for all configs.
add_compile_options(-ffunction-sections -fdata-sections)

# Link-time garbage collection and stripping for the main executable in Release
if(TARGET fun)
  # Garbage-collect unused sections; also strip symbols (-s) in Release
  target_link_options(fun PRIVATE
    $<$<CONFIG:Release>:-Wl,--gc-sections -s>
  )
  # Prefer enabling LTO/IPO for Release builds
  set_property(TARGET fun PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(TARGET fun_core)
  # LTO/IPO for the core library helps the final link DCE more Rust/C glue
  set_property(TARGET fun_core PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
  # Ensure objects compiled with sectioning (added globally above) benefit from GC
  target_link_options(fun_core PRIVATE $<$<CONFIG:Release>:-Wl,--gc-sections>)
endif()

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
