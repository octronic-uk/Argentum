/* 
 * This sotware is distributed under a standard, three-clause BSD license.
 * Please see the file LICENSE, distributed with this software, for specific
 * terms and conditions.
 */
#include <string.h>

char* strncpy(char *dest, const char *src, uint32_t n)
{
   char* tmp = (char*)dest;
    do {
        if (!n--)
            return dest;
    } while (*tmp++ = *src++);
    while (n--)
        *tmp++ = 0;
    return dest;
}
