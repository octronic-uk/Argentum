#/bin/bash

LIB_BUILD=_build

# Build Taskie Library
rm -rf ${BUILD_DIR}
mkdir ${LIB_BUILD}
cd ${LIB_BUILD}
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake

if [ "$?" -ne 0 ]; then
    echo "CMake Flopped :("
    exit
fi

make
MAKE_RESULT=$?
cd ..

if [ "$MAKE_RESULT" -ne 0 ]; then
    echo "Library Compilation Flopped :("
    exit
fi


# Build Kernel Executable
cd Boot
make clean
make 
MAKE_RESULT=$?
cd ..

if [ "$MAKE_RESULT" -ne 0 ]; then
    echo "Kernel Compilation/Linking Flopped ):"
    exit
fi


# Run in QEMU
qemu-system-i386 \
    -kernel Boot/TaskieKernel \
    -drive format=raw,if=ide,file=Tools/TaskieDisk.img \
    -serial stdio
