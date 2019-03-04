CC      = i386-elf-gcc
LD      = i386-elf-ld
NASM    = nasm 
LDFLAGS = -nostdlib -nostartfiles

SRC  = $(wildcard *.c)
SRC += $(wildcard Core/*.c)
SRC += $(wildcard Scheduler/*.c)
SRC += $(wildcard Task/*.c)

OBJ = $(SRC:.c=.o)


all : Boot/kmain_c.o Boot/kmain_asm.o OBJECTS 
	$(LD) -m elf_i386 -T Boot/link.ld -o TaskieKernel Boot/kmain_asm.o Boot/kmain_c.o $(OBJ)	

Boot/kmain_asm.o : 
	$(NASM) -f elf32 Boot/kmain.asm -o Boot/kmain_asm.o

Boot/kmain_c.o : 
	$(CC) -m32 -c Boot/kmain.c -o Boot/kmain_c.o

OBJECTS : $(OBJ)
    $(CC) -m32 -c $@ -o $@:.c=.o

clean:
	rm -rf TaskieKernel Boot/kmain_c.o Boot/kmain_asm.o $(OBJ)

run : all
	qemu-system-i386 -kernel TaskieKernel
