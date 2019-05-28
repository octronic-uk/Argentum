#!/bin/bash

export BASE=`pwd`
export TEST_DISK=${BASE}/Tools/MBR_FAT32_64m.img
export TEST_FLOPPY=${BASE}/Tools/MSD_Disk2.img
export KERNEL=${BASE}/Ag
export MEMORY=1G
export MAKEOPTS=-j8
export QEMU_BIN=qemu-system-i386
export MAKE_BIN=make
export GDB_BIN=/usr/local/Cellar/i386-elf-gdb/8.2/bin/i386-elf-gdb
export RUN_GDB="${GDB_BIN} $PWD/Ag -tui"
export RUN_MAKE="${MAKE_BIN} ${MAKEOPTS}"
#export RUN_QEMU="${QEMU_BIN} -m ${MEMORY} -kernel ${KERNEL} -drive format=raw,if=ide,file=${TEST_DISK} -drive format=raw,if=floppy,file=${TEST_FLOPPY} -serial stdio"
export RUN_QEMU="${QEMU_BIN} -m ${MEMORY} -kernel ${KERNEL} -drive format=raw,if=ide,file=${TEST_DISK} -serial stdio"
export RUN_QEMU_DEBUG="${RUN_QEMU} -s -S"
# ISO Specific
export ISO_DATE=`date +%Y-%m-%d_%H%M`
export ISO_DIR=${BASE}/iso
export ISO_BOOT=${ISO_DIR}/boot
export ISO_GRUB=${ISO_BOOT}/grub
export ISO_OUTPUT=${BASE}/Ag.iso

export RUN_QEMU_ISO="${QEMU_BIN} -boot d -cdrom ${ISO_OUTPUT} -m ${MEMORY} -drive format=raw,if=ide,file=${TEST_DISK} -serial stdio"
export RUN_QEMU_ISO_DEBUG="${RUN_QEMU_ISO} -s -S"