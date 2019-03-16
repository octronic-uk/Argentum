#/bin/bash
SZ=64
DISK=FAT32_${SZ}m.img

echo Creating Disk ${DISK}

dd if=/dev/zero of=./FAT32_${SZ}m.img bs=1 count=0 seek=${SZ}M

sudo fdisk ${DISK}

mkdosfs -F 32 -I ${DISK}
