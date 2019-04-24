# Project Binary
export KBIN=Argentum

# Compiler
export CC = i386-elf-gcc
export CFLAGS=-I$(PWD) -I$(PWD)/Clib/include -m32 -nostdinc  -nostdlib -nostartfiles

# Linker
export LD = i386-elf-ld
export LDFLAGS=--no-omagic -nostdlib -nostartfiles

#Assembler
export AS = i386-elf-as
export ASFLAGS=

# Boot must be last
SUBDIRS = Clib Drivers Objects Lua Boot

.PHONY: all clean

all clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile $@; \
	done
