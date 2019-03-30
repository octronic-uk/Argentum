#/bin/bash

./make.sh  

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

# Run in QEMU
qemu-system-i386 \
    -m 32 \
    -kernel Boot/TaskieKernel \
    -drive format=raw,if=ide,file=Tools/MBR_FAT32_64m.img \
    -serial stdio
