#/bin/bash

make clean
make 
qemu-system-i386 -kernel TaskieKernel
