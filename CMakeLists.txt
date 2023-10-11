cmake_minimum_required(VERSION 3.0.0)
project(shell VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

set(CMAKE_C_COMPILER "clang-16")
set(CMAKE_C_FLAGS "-std=gnu17 -Wvla -Wall -Wextra -Werror -Wpedantic -Wno-unused-result -Wconversion")

add_executable(shell shell.c command.c built-in.c shell_errors.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)