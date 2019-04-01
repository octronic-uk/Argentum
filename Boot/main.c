#include <Kernel.h>

struct multiboot_info_t;
struct Kernel kernel;

void kmain(multiboot_info_t* mbi)
{
    Kernel_Constructor(&kernel, mbi);
}
