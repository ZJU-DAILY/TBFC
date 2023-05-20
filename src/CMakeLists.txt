cmake_minimum_required(VERSION 3.15)
project(src)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g2 -ggdb -fopenmp -pthread")

file(GLOB SRC_LIST *)
# message(STATUS "SRC_LIST=${FRAM_SRC_LIST}")

add_library(src ${SRC_LIST})
set_target_properties(src PROPERTIES LINKER_LANGUAGE CXX)