LD = i386-elf-ld
AS = i386-elf-as
CC = i386-elf-gcc
LDFLAGS = -nostdlib -nostartfiles
CFLAGS=-I/usr/local/Cellar/i386-elf-gcc/8.2.0/lib/gcc/i386-elf/8.2.0/include -I../Core -m32 -nostdlib -nostartfiles 
#TASKIE_LIB=$(shell find ../_build -name "*.obj")
TASKIE_LIB=../_build/libTaskie.a

all : main_c.o main_asm.o
	$(LD) -T link.ld -melf_i386 main_asm.o main_c.o ${TASKIE_LIB} -o TaskieKernel  

main_asm.o :
	$(AS) main.s -o main_asm.o

main_c.o :
	$(CC) $(CFLAGS) -c main.c -o main_c.o

clean:
	rm -rf TaskieKernel main_c.o main_asm.o
