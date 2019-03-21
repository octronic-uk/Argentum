cmake_minimum_required(VERSION 3.0.0)
project(TaskieCore C)

file(GLOB_RECURSE SRC_FILES "*.c")

include_directories(Taskie ${CMAKE_SOURCE_DIR}/Core)
include_directories(Taskie ${CMAKE_SOURCE_DIR}/Core/LibC/include)

add_library(TaskieCore ${SRC_FILES})