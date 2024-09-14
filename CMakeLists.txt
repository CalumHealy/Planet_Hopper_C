cmake_minimum_required(VERSION 3.27)
project(space_explorer C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(space_explorer
        space_explorer.c
        space_explorer.h
        space_solution.c)
