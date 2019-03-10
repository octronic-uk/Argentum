CC      = i386-elf-gcc
CFLAGS  = -m32
LD      = i386-elf-ld
AS      = i386-elf-as
LDFLAGS = -nostdlib -nostartfiles

# /
SRC = $(wildcard *.c)
# /Core
SRC += $(wildcard Core/*.c)
SRC += $(wildcard Core/Memory/*.c)
SRC += $(wildcard Core/Keyboard/*.c)
SRC += $(wildcard Core/Interrupt/*.c)
SRC += $(wildcard Core/Screen/*.c)
SRC += $(wildcard Core/LinkedList/*.c)
# /Scheduler
SRC += $(wildcard Scheduler/*.c)

OBJ = $(SRC:c=o)
COMPILE=-c
OUTPUT=-o

all : Boot/kmain_c.o Boot/kmain_asm.o Core/Keyboard/Keyboard_asm.o $(OBJ)
	$(LD) -T Boot/link.ld -melf_i386 -o TaskieKernel Boot/kmain_asm.o Boot/kmain_c.o Core/Keyboard/Keyboard_asm.o $(OBJ)

Core/Keyboard/Keyboard_asm.o :
	$(AS) Core/Keyboard/Keyboard.s -o Core/Keyboard/Keyboard_asm.o

Boot/kmain_asm.o :
	$(AS) Boot/kmain.s -o Boot/kmain_asm.o

Boot/kmain_c.o :
	$(CC) $(CFLAGS) -c Boot/kmain.c -o Boot/kmain_c.o

OBJECTS : $(OBJ)
	$(CC) $(CFLAGS) $(COMPILE) ${@:o=c} $(OUTPUT) $@

clean:
	rm -rf TaskieKernel Boot/kmain_c.o Boot/kmain_asm.o Core/Keyboard/Keyboard_asm.o $(OBJ)

run :
	qemu-system-i386 -m 32M -kernel TaskieKernel
