# Common CMake defaults for Apollo IDE.

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Write compile_commands.json for Clang tooling and IDEs" FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif (MSVC)
    add_compile_options(/W4)
endif ()
