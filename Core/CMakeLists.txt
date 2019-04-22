cmake_minimum_required(VERSION 3.0.0)
project(ArgentumCore C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin")

file(GLOB_RECURSE SRC_FILES "*.c")

include_directories(
    ArgentumCore 
    ${CMAKE_SOURCE_DIR}/Core 
    ${CMAKE_SOURCE_DIR}/Core/Clib/include
    ${CMAKE_SOURCE_DIR}/Core/Drivers/ACPI/include
)

add_library(ArgentumCore ${SRC_FILES})