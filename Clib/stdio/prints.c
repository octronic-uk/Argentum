#include <stdio.h>

int prints(char **out, const char *string, int width, int flags)
{
	int pc = 0, padchar = ' ';

	if (width > 0)
    {
		int len = 0;
		const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (flags & PAD_ZERO)
			padchar = '0';
	}
	if (!(flags & PAD_RIGHT))
    {
		for ( ; width > 0; --width)
        {
			outputchar(out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string)
    {
		outputchar(out, *string);
		++pc;
	}
	for ( ; width > 0; --width)
    {
		outputchar(out, padchar);
		++pc;
	}

	return pc;
}