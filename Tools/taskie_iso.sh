#!/bin/bash

DATE=`date +%Y%m%d_%H%M`
BASE=`pwd`
ISO=${BASE}/t_iso
BOOT=${ISO}/boot
GRUB=${BOOT}/grub
OUTPUT=Taskie_${DATE}.iso
MACBOOK=Ashley@macbook

rm -rf $BASE/$ISO
mkdir -p ${ISO} ${BOOT} ${GRUB}
cd ${BOOT} 

scp ${MACBOOK}:/Volumes/Taskie/Boot/TaskieKernel .

cat << EOF > ${GRUB}/grub.cfg

menuentry "Taskie" {
    multiboot /boot/TaskieKernel
}
EOF

echo Creating ISO ${OUTPUT}
grub-mkrescue -o ${OUTPUT} ${ISO}

scp ${OUTPUT} ${MACBOOK}:Desktop/
