#include <Kernel.h>
#include <stdio.h>
// Lua
/*
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
*/

struct multiboot_info_t;
struct Kernel _Kernel;

void kmain(multiboot_info_t* mbi)
{
    Kernel_Constructor(&_Kernel, mbi);

    printf("Invoking Lua\n");
    //lua_State *L = lua_open();   /* opens Lua */
    //luaopen_base(L);             /* opens the basic library */
    //luaopen_table(L);            /* opens the table library */
    //luaopen_io(L);               /* opens the I/O library */
    //luaopen_string(L);           /* opens the string lib. */
    //luaopen_math(L);             /* opens the math lib. */

    printf("System Halted\n");
    asm("hlt");
}
