#/bin/bash

. Tools/env.sh

Tools/make_iso.sh

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

# Run in QEMU

${RUN_QEMU_ISO}  #> AgOut.txt

