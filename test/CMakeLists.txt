cmake_minimum_required(VERSION 3.5)
project(ymir_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O3 -ggdb"
)

enable_testing()

add_subdirectory(googletest)
add_subdirectory(cpp)

