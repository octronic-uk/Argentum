#/bin/bash

. Tools/env.sh

${RUN_MAKE} clean

${RUN_MAKE}

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

# Run in QEMU

${RUN_QEMU}  #> AgOut.txt

