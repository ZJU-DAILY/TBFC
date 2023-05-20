cmake_minimum_required(VERSION 3.15)
project(TBFC)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -fopenmp -pthread")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g2 -ggdb -fopenmp -pthread")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
add_subdirectory(src)

add_executable(reformat data_reformat.cc)

add_executable(main main.cc)
target_link_libraries(main src)