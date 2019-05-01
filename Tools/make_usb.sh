#!/bin/bash

. Tools/env.sh
${MAKE}

ssh ash@tc /nfs/mk_argentum_grub_iso.sh 
DEVICE=`diskutil list | grep "*61.5 GB" | sed -E 's/\s*0:\s*\w*\s*\*61.5\sGB\s*//g'`
DEVICE=/dev/r$DEVICE
echo Using Device $DEVICE
sudo dd if=/Users/Ashley/Desktop/ArgentumGrub.iso of=$DEVICE bs=1m
exit
