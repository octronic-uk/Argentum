#/bin/bash

_CMAKE_BUILD_DIR=_build
CMD_FLAGS_FILE=compile_commands.json

# Build Taskie Library
rm -rf ${_CMAKE_BUILD_DIR}
mkdir ${_CMAKE_BUILD_DIR}
cd ${_CMAKE_BUILD_DIR}
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake

if [ "$?" -ne 0 ]; then
    echo "CMake Flopped :("
    exit 1
fi

make
MAKE_RESULT=$?
cd ..

if [ "$MAKE_RESULT" -ne 0 ]; then
    echo "Library Compilation Flopped :("
    exit 1
fi

# Build Kernel Executable
cd Boot
make clean
make 
MAKE_RESULT=$?
cd ..

if [ "$MAKE_RESULT" -ne 0 ]; then
    echo "Kernel Compilation/Linking Flopped ):"
    exit 1
fi

# Copy Compile Flags for YCM
cp ${_CMAKE_BUILD_DIR}/${CMD_FLAGS_FILE} ./${CMD_FLAGS_FILE}
