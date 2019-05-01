#!/bin/bash

. Tools/env.sh

${MAKE}

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

cp /Volumes/ArgentumDev/Ag /Volumes/GRUB/Ag 
sync
diskutil unmount /Volumes/GRUB