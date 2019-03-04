CC      = i386-elf-gcc
LD      = i386-elf-ld
NASM    = nasm 
LDFLAGS = -nostdlib -nostartfiles

SRC  = $(wildcard *.c)
SRC += $(wildcard Scheduler/*.c)
SRC += $(wildcard Task/*.c)

OBJ = $(SRC:.c=.o)

all : Boot/kc.o Boot/kasm.o OBJECTS 
	$(LD) -m elf_i386 -T Boot/link.ld -o kernel Boot/kasm.o Boot/kc.o $(OBJ)	

Boot/kasm.o : 
	$(NASM) -f elf32 Boot/kernel.asm -o Boot/kasm.o

Boot/kc.o : 
	$(CC) -m32 -c Boot/kernel.c -o Boot/kc.o

OBJECTS : $(OBJ)
    $(CC) -m32 -c $@ -o $@:.c=.o

clean:
	rm -rf Boot/kc.o Boot/kasm.o $(OBJ)