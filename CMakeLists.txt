cmake_minimum_required(VERSION 3.7)
project(simd-test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")
# link_libraries("-march=native")

aux_source_directory(src SRC)
include_directories(src)

add_executable(mmul test/mmul.c ${SRC})