cmake_minimum_required(VERSION 3.2.1)
project (ipc)

include(CMakeToolsHelpers OPTIONAL)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(unittests)
add_subdirectory(tests)

#add_subdirectory(vendor/rttr)
