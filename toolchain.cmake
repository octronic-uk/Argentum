# i386-elf Toolchain File

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I/usr/local/Cellar/i386-elf-gcc/8.2.0/lib/gcc/i386-elf/8.2.0/include -ffreestanding -nostdlib")

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_CROSSCOMPILING TRUE)
set(triple i386-elf)

set(CMAKE_C_COMPILER "i386-elf-gcc")
set(CMAKE_CXX_COMPILER "i386-elf-g++") 

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

