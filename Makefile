TEST_FLAGS = -D__UNIT_TESTS 

DEBUG_FLAGS= -D__DEBUG_TASK \
	-D__DEBUG_PCI \
	-D__DEBUG_MEMORY \
	-D__DEBUG_LINKED_LIST \
	-D__DEBUG_SCHEDULER

CC = i386-elf-gcc 
CFLAGS  = -m32 $(TEST_FLAGS) $(DEBUG_FLAGS)
LD = i386-elf-ld
AS = i386-elf-as
LDFLAGS = -nostdlib -nostartfiles
QEMU_FLAG=- -m 32M -kernel TaskieKernel -hda Tools/FAT32_64m.img

# /
SRC = $(wildcard *.c)
# /Core
SRC += $(wildcard Core/*.c)
SRC += $(wildcard Core/ATA/*.c)
SRC += $(wildcard Core/Interrupt/*.c)
SRC += $(wildcard Core/IO/*.c)
SRC += $(wildcard Core/Keyboard/*.c)
SRC += $(wildcard Core/Memory/*.c)
SRC += $(wildcard Core/PCI/*.c)
SRC += $(wildcard Core/Screen/*.c)
# /DataStructures
SRC += $(wildcard DataStructures/LinkedList/*.c)
# /Filesystem
SRC += $(wildcard Filesystem/FAT/*.c)
# /Scheduler
SRC += $(wildcard Scheduler/*.c)

# Tests
#SRC += $(wildcard Core/ATA/Test/*.c)
SRC += $(wildcard DataStructures/LinkedList/Test/*.c)
#SRC += $(wildcard Filesystem/FAT/Test/*.c)

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
	qemu-system-i386  -m 32 -kernel TaskieKernel
