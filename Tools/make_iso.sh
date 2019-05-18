#!/bin/bash

. Tools/env.sh

${RUN_MAKE} clean

${RUN_MAKE}

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi

DATE=`date +%Y-%m-%d_%H%M`
ISO=${BASE}/iso
BOOT=${ISO}/boot
GRUB=${BOOT}/grub
OUTPUT=${BASE}/Ag_${DATE}.iso

rm -rf ${BASE}/${ISO}
mkdir -p ${ISO} ${BOOT} ${GRUB}
cd ${BOOT} 

cat << EOF > ${GRUB}/grub.cfg

menuentry "Argentum ${DATE}" {
    multiboot /boot/Ag
}
EOF

cp ${KERNEL} ${BOOT}

echo Creating ISO ${OUTPUT}
i386-elf-grub-mkrescue -o ${OUTPUT} ${ISO}
