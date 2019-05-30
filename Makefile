

# Compiler
export CC=i386-elf-gcc
export CFLAGS=-g -I$(PWD) -I$(PWD)/External/musl/include -m32 -nostdinc  -nostdlib -nostartfiles -fno-omit-frame-pointer

# Linker
export LD=i386-elf-ld
export LDFLAGS=--no-omagic -nostdlib -nostartfiles

#Assembler
export AS=i386-elf-as
export ASFLAGS=

# Boot must be last
SUBDIRS = Drivers Objects LibC Boot

.PHONY: all clean

all clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile $@; \
	done
