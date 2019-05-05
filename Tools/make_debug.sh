#/bin/bash

. Tools/env.sh

#${RUN_MAKE} clean

${RUN_MAKE}

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

echo
echo Now run ${RUN_GDB} 
echo "(gdb) target remote :1234"
echo

# Run in QEMU
${RUN_QEMU_DEBUG} 
