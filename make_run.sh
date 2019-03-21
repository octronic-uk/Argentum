#/bin/bash

./make.sh  

# Run in QEMU
qemu-system-i386 \
    -kernel Boot/TaskieKernel \
    -drive format=raw,if=ide,file=Tools/TaskieDisk.img \
    -serial stdio
