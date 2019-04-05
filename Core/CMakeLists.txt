cmake_minimum_required(VERSION 3.0.0)
project(TaskieCore C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

file(GLOB_RECURSE SRC_FILES "*.c")

include_directories(
    TaskieCore 
    ${CMAKE_SOURCE_DIR}/Core 
    ${CMAKE_SOURCE_DIR}/Core/Clib/include
    ${CMAKE_SOURCE_DIR}/Core/Drivers/ACPI/include
)

add_library(TaskieCore ${SRC_FILES})