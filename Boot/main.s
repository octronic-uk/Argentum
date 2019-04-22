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

.section .bss
.align 16
stack_bottom:
.skip 1024*1024*4 # Stack size
stack_top:



.global fpy_dma_buf 

# The kernel entry point.
.section .text

.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
 
.global irq0_handler
.global irq1_handler
.global irq2_handler
.global irq3_handler
.global irq4_handler
.global irq5_handler
.global irq6_handler
.global irq7_handler
.global irq8_handler
.global irq9_handler
.global irq10_handler
.global irq11_handler
.global irq12_handler
.global irq13_handler
.global irq14_handler
.global irq15_handler
 
.extern irq0_handler
.extern irq1_handler
.extern irq2_handler
.extern irq3_handler
.extern irq4_handler
.extern irq5_handler
.extern irq6_handler
.extern irq7_handler
.extern irq8_handler
.extern irq9_handler
.extern irq10_handler
.extern irq11_handler
.extern irq12_handler
.extern irq13_handler
.extern irq14_handler
.extern irq15_handler
 
irq0:
  pusha
  call irq0_handler
  popa
  iret
 
irq1:
  pusha
  call irq1_handler
  popa
  iret
 
irq2:
  pusha
  call irq2_handler
  popa
  iret
 
irq3:
  pusha
  call irq3_handler
  popa
  iret
 
irq4:
  pusha
  call irq4_handler
  popa
  iret
 
irq5:
  pusha
  call irq5_handler
  popa
  iret
 
irq6:
  pusha
  call irq6_handler
  popa
  iret
 
irq7:
  pusha
  call irq7_handler
  popa
  iret
 
irq8:
  pusha
  call irq8_handler
  popa
  iret
 
irq9:
  pusha
  call irq9_handler
  popa
  iret
 
irq10:
  pusha
  call irq10_handler
  popa
  iret
 
irq11:
  pusha
  call irq11_handler
  popa
  iret
 
irq12:
  pusha
  call irq12_handler
  popa
  iret
 
irq13:
  pusha
  call irq13_handler
  popa
  iret
 
irq14:
  pusha
  call irq14_handler
  popa
  iret
 
irq15:
  pusha
  call irq15_handler
  popa
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

.section .floppy_dma_buffer
fpy_dma_buf:
.skip 512 # 1024*64  64K Buffer


#
# arch/i386/setjmp.S
#
# setjmp/longjmp for the i386 architecture
#

#
# The jmp_buf is assumed to contain the following, in order:
#	%ebx
#	%esp
#	%ebp
#	%esi
#	%edi
#	<return address>
#

.text
.align 4
.globl kernel_setjmp
.type kernel_setjmp, @function
kernel_setjmp:
#ifdef _REGPARM
	movl %eax,%edx
#else
	movl 4(%esp),%edx
#endif
	popl %ecx			# Return address, and adjust the stack
	xorl %eax,%eax			# Return value
	movl %ebx,(%edx)
	movl %esp,4(%edx)		# Post-return %esp!
	pushl %ecx			# Make the call/return stack happy
	movl %ebp,8(%edx)
	movl %esi,12(%edx)
	movl %edi,16(%edx)
	movl %ecx,20(%edx)		# Return address
	ret

.size kernel_setjmp,.-kernel_setjmp

.text
.align 4
.globl kernel_longjmp
.type kernel_longjmp, @function

kernel_longjmp:
#ifdef _REGPARM
	xchgl %eax,%edx
#else
	movl 4(%esp),%edx		# jmp_ptr address
	movl 8(%esp),%eax		# Return value
#endif
	movl (%edx),%ebx
	movl 4(%edx),%esp
	movl 8(%edx),%ebp
	movl 12(%edx),%esi
	movl 16(%edx),%edi
	jmp *20(%edx)

.size kernel_longjmp,.-kernel_longjmp

