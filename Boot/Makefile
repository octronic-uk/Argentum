LD = i386-elf-ld
AS = i386-elf-as
CC = i386-elf-gcc
LDFLAGS=--no-omagic -nostdlib -nostartfiles
ASFLAGS=
CFLAGS=-nostdinc -I../Core -I../Core/Clib/include -m32  -nostdlib -nostartfiles 
ARGENTUM_LIB=../_build/Core/libArgentumCore.a
LUA_LIB=../_build/Lua/libLua.a
KBIN=Argentum

all : main_c.o main_asm.o
	$(LD) $(LDFLAGS) -T link.ld -melf_i386 main_asm.o main_c.o ${ARGENTUM_LIB} ${LUA_LIB} -o  ${KBIN}

main_asm.o :
	$(AS) $(ASFLAGS) main.s -o main_asm.o

main_c.o :
	$(CC) $(CFLAGS) -c main.c -o main_c.o

clean:
	rm -rf ${KBIN} *.o
