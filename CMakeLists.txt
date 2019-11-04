cmake_minimum_required(VERSION 3.2.1)
project (ipc)

include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(unittests)
add_subdirectory(tests)