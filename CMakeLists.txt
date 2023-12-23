cmake_minimum_required(VERSION 3.20)
project(ymir)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT WIN32)
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -ggdb"
  )
endif()

add_subdirectory(lib)

option(BUILD_TESTS "If to build unit tests (needs google-test)" OFF)
if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

option(BUILD_EXAMPLES "If to build examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()