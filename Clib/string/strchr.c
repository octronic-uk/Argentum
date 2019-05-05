#include <string.h>

char* strchr(const char* str, int character)
{
    int i = 0;
    while (str[i])
    {
        if (str[i] == (char)character)
        {
            return (char*)&str[i];
        }
        i++;
    }
    return 0;
}