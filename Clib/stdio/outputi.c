#include <stdio.h>

int outputi(char **out, long i, int base, int sign, int width, int flags, int letbase)
{
	char print_buf[BUFSIZ];
	char *s;
	int t, neg = 0, pc = 0;
	unsigned long u = i;

	if (i == 0)
    {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, flags);
	}

	if (sign && base == 10 && i < 0)
    {
		neg = 1;
		u = -i;
	}

	s = print_buf + BUFSIZ-1;
	*s = '\0';

	while (u)
    {
		t = u % base;
		if( t >= 10 )
        {
			t += letbase - '0' - 10;
        }
		*--s = t + '0';
		u /= base;
	}

	if (neg)
    {
		if( width && (flags & PAD_ZERO) )
        {
			outputchar (out, '-');
			++pc;
			--width;
		}
		else
        {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, flags);
}