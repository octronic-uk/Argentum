#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
// Lua
#include <Lua/lua.h>
#include <Lua/lualib.h>

struct multiboot_info_t;
struct Kernel _Kernel;

static int argc = 1;
static char* argv[1];
static char* arg0 = "Argentum";

void kmain(multiboot_info_t* mbi)
{
    argv[0] = arg0;

    if( Kernel_Constructor(&_Kernel, mbi))
    {
        printf("Ag: Invoking Lua\n");
        lua_main(argc,argv);
    }
    else
    {
        printf("Ag: Fatal Error - Kernel Construction Failed\n");
    }
    
    /*
    printf("Ag: System Halted\n");

    asm("cli");
    asm("hlt");
    */
}
