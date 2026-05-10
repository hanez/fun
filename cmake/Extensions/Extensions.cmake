# Toggleable optional features live in per-file modules. These populate *_INCLUDE_DIRS and *_LINK_LIBS
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Helper to report feature status nicely
function(_fun_print_feature name flag)
  if(${flag})
    message(STATUS "  ${name}: ENABLED")
  else()
    message(STATUS "  ${name}: DISABLED")
  endif()
endfunction()

# Include each extension module
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/CURL.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/INI.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/JSON.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/KCGI.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/OPENSSL.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/PCRE2.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/PCSC.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/REPL.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/SQLITE.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Extensions/XML2.cmake)

# Summary of extension toggles
message(STATUS "---- Fun extension summary ----")
_fun_print_feature("curl (FUN_WITH_CURL)" FUN_WITH_CURL)
_fun_print_feature("iniparser (FUN_WITH_INI)" FUN_WITH_INI)
_fun_print_feature("json-c (FUN_WITH_JSON)" FUN_WITH_JSON)
_fun_print_feature("kcgi (FUN_WITH_KCGI)" FUN_WITH_KCGI)
_fun_print_feature("libxml2 (FUN_WITH_XML2)" FUN_WITH_XML2)
_fun_print_feature("OpenSSL (FUN_WITH_OPENSSL)" FUN_WITH_OPENSSL)
_fun_print_feature("PCRE2 (FUN_WITH_PCRE2)" FUN_WITH_PCRE2)
_fun_print_feature("PCSC-Lite (FUN_WITH_PCSC)" FUN_WITH_PCSC)
_fun_print_feature("REPL (FUN_WITH_REPL)" FUN_WITH_REPL)
_fun_print_feature("SQLite (FUN_WITH_SQLITE)" FUN_WITH_SQLITE)
message(STATUS "--------------------------------")
