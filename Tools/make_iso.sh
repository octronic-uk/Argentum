#!/bin/bash

. Tools/env.sh

${RUN_MAKE} clean

${RUN_MAKE}

if [ "$?" -ne 0 ]; then
    echo Make failed
    exit 1
fi



rm -rf ${ISO_DIR}
mkdir -p ${ISO_DIR} ${ISO_BOOT} ${ISO_GRUB}

cat << EOF > ${ISO_GRUB}/grub.cfg

set timeout=1

menuentry "Argentum ${ISO_DATE}" {
    multiboot /boot/Ag
    set gfxpayload=800x600x32
    boot
}
EOF

cp ${KERNEL} ${ISO_BOOT}

echo Creating ISO ${ISO_OUTPUT}
i386-elf-grub-mkrescue -o ${ISO_OUTPUT} ${ISO_DIR}
