#include <stdio.h>

void outputchar(char *str, char c)
{
	if (str)
    {
		*str = c;
		++(str);
	}
    else
    {
		putchar(c);
	}
}