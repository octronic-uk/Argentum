#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <Lua/lua.h>
#include <Lua/lualib.h>
#include <Lua/lauxlib.h>

struct multiboot_info_t;
struct Kernel _Kernel;

static void* l_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
  (void)ud;
  (void)osize;
  if (nsize == 0)
  {
    MemoryDriver_Free(&_Kernel.Memory, ptr);
    return NULL;
  }
  else
  {
    return MemoryDriver_Reallocate(&_Kernel.Memory, ptr, nsize);
  }
}

int argc = 2;
char *argv[2];
char* arg0 = "Argentum";
char* arg1 = "-v";

void kmain_run_lua()
{
  lua_State *L = luaL_newstate();  /* create state */
  lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
  luaL_openlibs(L);  /* open libraries */
  lua_gc(L, LUA_GCRESTART, 0);
  dotty(L);
  lua_close(L);
}

void kmain(multiboot_info_t* mbi)
{

    if( !Kernel_Constructor(&_Kernel, mbi))
    {
        printf("Ag: Fatal Error - Kernel Construction Failed\n");
        abort();
    }

    // kmain_run_lua();

    Kernel_Destructor(&_Kernel);
    
    printf("Ag: Finished! - System Halted\n");
    abort();
}
