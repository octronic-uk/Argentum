#include <string.h>

#include <stdint.h>
#include <stdbool.h>

bool in_base(char ch, int base)
{
    switch (base)
    {
        case 8:
            return ch >= '0' && ch <= '7';
        case 10:
            return ch >= '0' && ch <= '9';
        case 16:
            return ch >= '0' && ch <= '9' ||
                   ch >= 'A' && ch <= 'F' ||
                   ch >= 'a' && ch <= 'f';
        default:
            return 0;
    }
}

int char_to_int(char ch, int base)
{
    switch (base)
    {
        case 8:
        case 10:
            return ch -  '0';
        case 16:
            if (ch >= '0' && ch <= '9' )
            {
                return ch - '0';
            }
            else if (ch >= 'A' && ch <= 'F')
            {
                return ch - 'A';
            }
            else if (ch >= 'a' && ch <= 'f')
            {
                return ch - 'f';
            }
        default:
            return 0;
    }
}

long int string_to_long(const char* nptr, char** endptr, int base)
{
    size_t i;
    int32_t sum;
    bool neg;

    i = 0;
    sum = 0;

    while (is_space(nptr[i])) 
    {
        i++;
    }

    if (nptr[i] == '+') 
    {
        neg = false;
        i++;
    }
    else if (nptr[i] == '-') 
    {
        neg = true;
        i++;
    }

    if (base == 0) 
    {
        if (nptr[i] == '0') 
        {
            i++;
            if (nptr[i] == 'x') 
            {
                base = 16;
            }
            else if (in_base(nptr[i],8)) 
            {
                base = 8;
            }
        }
        else
        {
            base = 10;
        }
    }

    if (base == 0) 
    {
        if (endptr) 
        {
            *endptr = (char*) &nptr[i];
        }
        return 0;
    }

    while (nptr[i] && in_base(nptr[i], base)) 
    {
        sum *= base;
        sum += char_to_int(nptr[i], base);
        i++;
    }

    if (endptr) 
    {
        *endptr = (char*) &nptr[i];
    }

    return (neg) ? -sum : sum;
}

