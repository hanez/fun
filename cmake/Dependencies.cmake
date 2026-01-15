# Common dependency discovery used across the project

# Threads (used by demos and optionally by the runtime)
if(UNIX)
  find_package(Threads QUIET)
endif()

# Python for helper scripts
find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
  set(_FUN_PY "${Python3_EXECUTABLE}")
else()
  set(_FUN_PY "python3")
endif()
