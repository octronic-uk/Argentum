

# Compiler
export CC=i386-elf-gcc
export CFLAGS=-g -I$(PWD) -I$(PWD)/Clib/include -m32 -nostdinc  -nostdlib -nostartfiles -fno-omit-frame-pointer

# Linker
export LD=i386-elf-ld
export LDFLAGS=--no-omagic -nostdlib -nostartfiles

#Assembler
export AS=i386-elf-as
export ASFLAGS=-g

# Boot must be last
SUBDIRS = Clib Drivers Objects Boot 

.PHONY: all clean

all clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f Makefile $@; \
	done
