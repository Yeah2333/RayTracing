cmake_minimum_required( VERSION 3.1.0 )

project( RayTracing )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread ")

set( CMAKE_CXX_STANDARD 11)

include_directories(include)
include(cmake/stb.cmake)


add_executable(main src/main.cpp)