#/bin/bash

TEST_DISK=Tools/MBR_FAT32_64m.img
KERNEL=Boot/Argentum

./make.sh  

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

# Run in QEMU
qemu-system-i386 \
    -m 32 \
    -kernel ${KERNEL} \
    -drive format=raw,if=ide,file=$TEST_DISK \
    -serial stdio
