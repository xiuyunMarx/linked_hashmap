cmake_minimum_required(VERSION 3.22)
project(linked_hashmap)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(linked_hashmap
        exceptions.hpp
        linked_hashmap.hpp
        utility.hpp
        7.cpp)
