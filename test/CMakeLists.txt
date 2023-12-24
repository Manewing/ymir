cmake_minimum_required(VERSION 3.20)
project(ymir_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

if (NOT WIN32)
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -ggdb"
  )
endif()


add_subdirectory(cpp)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

