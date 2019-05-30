#!/bin/bash

CC=i386-elf-gcc
AR=i386-elf-ar
LD=i386-elf-ld
CFLAGS="-std=gnu99 -static -fno-PIC -nostdlib -nodefaultlibs -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -masm=intel -O1 -g -c"
CONF_FLAGS="--disable-shared --enable-debug"
MAKE_FLAGS=-j8

cd External
rm -rf musl
git clone git://git.musl-libc.org/musl
cd musl
make clean
./configure LD=${LD} AR=${AR} CC=${CC} ${CONF_FLAGS}
make V=1 ${MAKE_FLAGS}
