#/bin/bash

./make.sh  

GDB=/usr/local/Cellar/i386-elf-gdb/8.2/bin/i386-elf-gdb

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

echo
echo Now run $GDB $PWD/Boot/TaskieKernel -tui
echo "(gdb) target remote :1234"
echo

# Run in QEMU
qemu-system-i386 \
    -m 32 \
    -kernel Boot/TaskieKernel \
    -drive format=raw,if=ide,file=Tools/TaskieDisk.img \
    -serial stdio \
    -s -S -d int
