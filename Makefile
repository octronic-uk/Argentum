CC      = i386-elf-gcc
LD      = i386-elf-ld
NASM    = nasm 
LDFLAGS = -nostdlib -nostartfiles

# /
SRC  = $(wildcard *.c)
# /Core
SRC += $(wildcard Core/*.c)
SRC += $(wildcard Core/Memory/*.c)
SRC += $(wildcard Core/Keyboard/*.c)
SRC += $(wildcard Core/Interrupt/*.c)
SRC += $(wildcard Core/Screen/*.c)
# /Scheduler
SRC += $(wildcard Scheduler/*.c)
#/Task
SRC += $(wildcard Task/*.c)

OBJ = $(SRC:.c=.o)


all : Boot/kmain_c.o Boot/kmain_asm.o Core/Interrupt/Interrupt_asm.o Core/Keyboard/Keyboard_asm.o OBJECTS 
	$(LD) -m elf_i386 -T Boot/link.ld -o TaskieKernel \
		Boot/kmain_asm.o \
		Boot/kmain_c.o  \
		Core/Interrupt/Interrupt_asm.o \
		Core/Keyboard/Keyboard_asm.o \
		$(OBJ)	

Core/Interrupt/Interrupt_asm.o :
	$(NASM) -f elf32 Core/Interrupt/Interrupt.asm -o Core/Interrupt/Interrupt_asm.o

Core/Keyboard/Keyboard_asm.o :
	$(NASM) -f elf32 Core/Keyboard/Keyboard.asm -o Core/Keyboard/Keyboard_asm.o

Boot/kmain_asm.o : 
	$(NASM) -f elf32 Boot/kmain.asm -o Boot/kmain_asm.o

Boot/kmain_c.o : 
	$(CC) -m32 -c Boot/kmain.c -o Boot/kmain_c.o

OBJECTS : $(OBJ)
    $(CC) -m32 -c $@ -o $@:.c=.o

clean:
	rm -rf TaskieKernel Boot/kmain_c.o Boot/kmain_asm.o Core/Keyboard/Keyboard_asm.o Core/Interrupt_asm.o $(OBJ)

run : all
	qemu-system-i386 -kernel TaskieKernel
