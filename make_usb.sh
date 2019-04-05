#!/bin/bash

./make.sh
ssh ash@tc /nfs/mk_taskie_grub_iso.sh 
DEVICE=`diskutil list | grep "*61.5 GB" | sed -E 's/\s*0:\s*\w*\s*\*61.5\sGB\s*//g'`
DEVICE=/dev/r$DEVICE
echo Using Device $DEVICE
sudo dd if=/Users/Ashley/Desktop/Taskie_Grub.iso of=$DEVICE bs=1m
exit
