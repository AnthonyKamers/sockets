cmake_minimum_required(VERSION 3.23)
project(sockets C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(server server.c)
add_executable(client client.c)
