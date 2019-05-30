#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
#include <stdlib.h>

struct multiboot_info_t;
struct Kernel _Kernel;

void kmain(multiboot_info_t* mbi)
{

    if( !Kernel_Constructor(&_Kernel, mbi))
    {
        printf("Ag: Fatal Error - Kernel Construction Failed\n");
        abort();
    }

    if (!Kernel_Init(&_Kernel))
    {
        printf("Ag: Fatal Error - Kernel Init Failed\n");
        abort();
    }

    Kernel_Run(&_Kernel);

    Kernel_Destructor(&_Kernel);
    
    printf("Ag: Finished! - System Halted\n");
    abort();
}
