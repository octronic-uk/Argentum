#/bin/bash

LIB_BUILD=_build

# Build Taskie Library
rm -rf ${BUILD_DIR}
mkdir ${LIB_BUILD}
cd ${LIB_BUILD}
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
make
cd ..

# Build Kernel Executable
cd Boot
make clean
make 
cd ..

# Run in QEMU
#qemu-system-i386 -kernel Boot/TaskieKernel -drive format=raw,file=Tools/TaskieDisk.img -monitor stdio
qemu-system-i386 -kernel Boot/TaskieKernel -drive format=raw,file=Tools/TaskieDisk.img
