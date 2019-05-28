# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set MEMGFX,   1<<2             # request gfx info
.set FLAGS,    ALIGN | MEMINFO | MEMGFX  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
# Graphics mode settings
.long 0
.long 0
.long 0
.long 0 
.long 0
.long 0
.long 800 # width
.long 600 # height
.long 32  # bpp

.section .bss
.align 16
stack_bottom:
.skip 1024*1024*2 # Stack size
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

/*****************************************************************************/
/* setjump, longjump                                                         */
/*****************************************************************************/

/*
.text
.align 4
*/
.globl setjmp
.type setjmp,@function
setjmp:
  mov    4(%esp), %eax     /* get pointer to jmp_buf, passed as argument on stack */ 
  mov    %ebx,    (%eax)   /* jmp_buf[0] = ebx */
  mov    %esi,    4(%eax)  /* jmp_buf[1] = esi */
  mov    %edi,    8(%eax)  /* jmp_buf[2] = edi */
  mov    %ebp,    12(%eax) /* jmp_buf[3] = ebp */
  lea    4(%esp), %ecx     /* get previous value of esp, before call */
  mov    %ecx,    16(%eax) /* jmp_buf[4] = esp before call */
  mov    (%esp),  %ecx     /* get saved caller eip from top of stack */
  mov    %ecx,    20(%eax) /* jmp_buf[5] = saved eip */
  xor    %eax,    %eax     /* eax = 0 */
  ret                      /* pop stack into eip */

/*
.text
.align 4
*/
.globl longjmp

longjmp:
  mov  4(%esp),%edx /* get pointer to jmp_buf, passed as argument 1 on stack */
  mov  8(%esp),%eax /* get int val in eax, passed as argument 2 on stack */
  test    %eax,%eax /* is int val == 0? */
  jnz lj1
  inc     %eax      /* if so, eax++ */
lj1:
  mov   (%edx),%ebx /* ebx = jmp_buf[0] */
  mov  4(%edx),%esi /* esi = jmp_buf[1] */
  mov  8(%edx),%edi /* edi = jmp_buf[2] */
  mov 12(%edx),%ebp /* ebp = jmp_buf[3] */
  mov 16(%edx),%ecx /* ecx = jmp_buf[4] */
  mov     %ecx,%esp /* esp = ecx */
  mov 20(%edx),%ecx /* ecx = jmp_buf[5] */
  jmp *%ecx         /* eip = ecx */

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
  xor %ebp, %ebp       # Set %ebp to NULL
	push %ebx
	call kmain

	# Hang if kernel_main unexpectedly returns.
	# cli
hlt_loop:	hlt
	jmp hlt_loop 

.size _start, . - _start

.section .floppy_dma_buffer
fpy_dma_buf:
.skip 512 # 1024*64  64K Buffer


