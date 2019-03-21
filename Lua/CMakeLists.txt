cmake_minimum_required(VERSION 3.0.0)

project(TaskieLua C)

file(GLOB_RECURSE SRC_FILES "*.c")

include_directories(TaskieLua ${CMAKE_SOURCE_DIR}/Core/LibC/include)

add_library(TaskieLua ${SRC_FILES})