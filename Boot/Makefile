LD = i386-elf-ld
AS = i386-elf-as
CC = i386-elf-gcc
LDFLAGS = -nostdlib -nostartfiles
CFLAGS=-g  -nostdinc -I../Core -I../Core/LibC/include -m32  -nostdlib -nostartfiles 
TASKIE_LIB=../_build/Core/libTaskieCore.a

all : main_c.o main_asm.o
	$(LD) -T link.ld -melf_i386 main_asm.o main_c.o ${TASKIE_LIB} -o TaskieKernel  

main_asm.o :
	$(AS) -g main.s -o main_asm.o

main_c.o :
	$(CC) $(CFLAGS) -c main.c -o main_c.o

clean:
	rm -rf TaskieKernel main_c.o main_asm.o
