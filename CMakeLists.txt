cmake_minimum_required( VERSION 3.1.0 )

project( RayTracing )

set( CMAKE_CXX_STANDARD 11)

include_directories(include)

link_libraries(pthread)

add_executable(main main.cpp)