LD = i386-elf-ld
AS = i386-elf-as
CC = i386-elf-gcc
LDFLAGS=-nostdlib -nostartfiles
ASFLAGS=
CFLAGS=-nostdinc -I../Core -I../Core/Clib/include -m32  -nostdlib -nostartfiles 
ARGENTUM_LIB=../_build/Core/libArgentumCore.a
KBIN=Argentum

all : main_c.o main_asm.o
	$(LD) -T link.ld -melf_i386 main_asm.o main_c.o ${ARGENTUM_LIB} -o  ${KBIN}

main_asm.o :
	$(AS) $(ASFLAGS) main.s -o main_asm.o

main_c.o :
	$(CC) $(CFLAGS) -c main.c -o main_c.o

clean:
	rm -rf ${KBIN} *.o
