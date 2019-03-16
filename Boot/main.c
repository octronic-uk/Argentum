#include "../Core/Kernel.h"

struct multiboot_info_t;

void kmain(multiboot_info_t* mbi)
{
    Kernel_Constructor(mbi);
}
