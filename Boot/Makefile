DRIVERS = $(shell find ../Drivers -type f -name "*.o")
OBJECTS = $(shell find ../Objects -type f -name "*.o") 
LUA     = $(shell find ../Lua     -type f -name "*.o")
CLIB    = $(shell find ../Clib    -type f -name "*.o")

all : main_c.o main_asm.o
	$(LD) $(LDFLAGS) -T link.ld -m elf_i386 main_asm.o main_c.o $(CLIB) $(DRIVERS) $(OBJECTS) $(LUA) -o ${KBIN}

main_asm.o :
	$(AS) $(ASFLAGS) main.s -o main_asm.o

main_c.o :
	$(CC) $(CFLAGS) -c main.c -o main_c.o

clean:
	rm -rf ${KBIN} *.o