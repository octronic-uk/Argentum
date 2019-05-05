#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
#include <stdlib.h>

struct multiboot_info_t;
struct Kernel _Kernel;

void kmain(multiboot_info_t* mbi)
{
    if( Kernel_Constructor(&_Kernel, mbi))
    {
        printf("Ag: Invoking Lua\n");
    }
    else
    {
        printf("Ag: Fatal Error - Kernel Construction Failed\n");
        abort();
    }
    
    printf("Ag: System Halted\n");
    abort();
}
