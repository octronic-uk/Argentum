#include <stdint.h>
#include <string.h>

char* string_concatenate(char* dest, const char* src)
{
    // Find where to append
    uint32_t end_of_dest = 0;
    while(dest[end_of_dest] != 0) 
    { 
        end_of_dest++; 
    }

    // Find length to append
    uint32_t size_of_src = 0;
    while (src[size_of_src] != 0)
    {
        size_of_src++;
    }

    // Append src to dest
    memory_copy(&dest[end_of_dest], src, size_of_src);
    
    // Null Terminate
    dest[end_of_dest + size_of_src]  = 0;
    return dest;
}
