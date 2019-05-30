#!/bin/bash

#MAKE_FLAGS=-j5 LDFLAGS="-m64 -fno-PIC" CC="gcc -m64 -O0 -g -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -fno-PIC -static" ansi

#CFLAGS="-std=gnu99 -static -fno-PIC -nostdlib -nodefaultlibs -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -masm=intel -O1 -g -c"
MAKE_FLAGS=-j5 
LDFLAGS=-fno-PIC
CC=i386-elf-gcc
CFLAGS="-O0 -g -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -fno-PIC -static"
LUA_VERSION=5.2.4

cd External

rm -rf lua-${LUA_VERSION}
rm -rf lua-${LUA_VERSION}.tar.gz

wget https://www.lua.org/ftp/lua-${LUA_VERSION}.tar.gz

tar -xvf lua-${LUA_VERSION}.tar.gz

cd lua-${LUA_VERSION}
make ${MAKE_FLAGS} CC=${CC} ansi
