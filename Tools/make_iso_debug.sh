#/bin/bash

. Tools/env.sh

Tools/make_iso.sh

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

echo
echo Now run ${RUN_GDB} 
echo "(gdb) target remote :1234"
echo

# Run in QEMU
${RUN_QEMU_ISO_DEBUG} 
