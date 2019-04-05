#include <Kernel.h>
#include <stdio.h>

struct multiboot_info_t;

void kmain(multiboot_info_t* mbi)
{
    struct Kernel kernel;
    Kernel_Constructor(&kernel, mbi);
    printf("System Halted\n");
    asm("hlt");
}
