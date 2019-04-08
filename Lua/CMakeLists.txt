cmake_minimum_required(VERSION 3.0.0)

project(Lua C)

file(GLOB_RECURSE SRC_FILES "*.c")

include_directories(Lua ${CMAKE_SOURCE_DIR}/Core/Clib/include)

add_library(Lua ${SRC_FILES})