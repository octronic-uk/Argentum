cmake_minimum_required(VERSION 3.0.0)

project(Taskie C)

set(CMAKE_VERBOSE_MAKEFILE ON)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -nostartfiles -nostdlib")

file(
	GLOB_RECURSE
	SRC_FILES
	"Core/*.c"
)

include_directories(
	Taskie
	${CMAKE_SOURCE_DIR}/Core
)

add_library(
    Taskie
    ${SRC_FILES}
)
