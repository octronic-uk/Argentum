# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text

.global _start
.type _start, @function

_start:
	movl $stack_top, %esp

	# Load our own GDT as the multioot GDT record may be invalid
    call load_gdt
	# Transfer control to the main kernel.
	push %ebx
	call kmain

	# Hang if kernel_main unexpectedly returns.
	# cli
1:	hlt
	jmp 1b

.size _start, . - _start

.global Serial_Port1InterruptHandlerASM
.extern Serial_Port1InterruptHandler

Serial_Port1InterruptHandlerASM:
	call Serial_Port1InterruptHandler
	iret

.global I8042_FirstPortInterruptHandlerASM
.extern I8042_FirstPortInterruptHandler

I8042_FirstPortInterruptHandlerASM:
	call I8042_FirstPortInterruptHandler
	iret

.global I8042_SecondPortInterruptHandlerASM
.extern I8042_SecondPortInterruptHandler

I8042_SecondPortInterruptHandlerASM:
	call I8042_SecondPortInterruptHandler
	iret




# GDT with a NULL Descriptor, a 32-Bit code Descriptor
# and a 32-bit Data Descriptor
gdt_start:
gdt_null:
    .long 0x0
    .long 0x0

gdt_code:
    .short 0xffff
    .short 0x0
    .byte 0x0
    .byte 0b10011010
    .byte 0b11001111
    .byte 0x0

gdt_data:
    .short 0xffff
    .short 0x0
    .byte 0x0
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x0
gdt_end:

# GDT descriptor record
gdtr:
    .short gdt_end - gdt_start - 1
    .long gdt_start

CODE_SEL = gdt_code - gdt_start
DATA_SEL = gdt_data - gdt_start

# Load GDT and set selectors for a flat memory model
load_gdt:
    lgdt (gdtr)
    ljmp $CODE_SEL, $.setcs             # Set CS selector with far JMP
.setcs:
    mov $DATA_SEL, %eax                 # Set the Data selectors to defaults
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %ss
    mov %eax, %fs
    mov %eax, %gs
    ret

.global load_gdt
.section .data
