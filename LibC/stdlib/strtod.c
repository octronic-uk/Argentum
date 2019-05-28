#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


double strtod(const char *str, char **end) 
{
    double d = 0.0;
    int sign;
    int n = 0;
    const char *p, *a;

    a = p = str;
    p = skipwhite(p);

    /* decimal part */
    sign = 1;
    if (*p == '-')
    {
	sign = -1;
	++p;
    } else if (*p == '+')
	++p;
    if (isdigit(*p))
    {
	d = (double)(*p++ - '0');
	while (*p && isdigit(*p))
	{
	    d = d * 10.0 + (double)(*p - '0');
	    ++p;
	    ++n;
	}
	a = p;
    } else if (*p != '.')
	goto done;
    d *= sign;

    /* fraction part */
    if (*p == '.')
    {
	double f = 0.0;
	double base = 0.1;
	++p;

	if (isdigit(*p))
	{
	    while (*p && isdigit(*p))
	    {
		f += base * (*p - '0') ;
		base /= 10.0;
		++p;
		++n;
	    }
	}
	d += f * sign;
	a = p;
    }

    /* exponential part */
    if ((*p == 'E') || (*p == 'e'))
    {
	int e = 0;
	++p;

	sign = 1;
	if (*p == '-')
	{
	    sign = -1;
	    ++p;
	} else if (*p == '+')
	    ++p;

	if (isdigit(*p))
	{
	    while (*p == '0')
		++p;
	    e = (int)(*p++ - '0');
	    while (*p && isdigit(*p))
	    {
		e = e * 10 + (int)(*p - '0');
		++p;
	    }
	    e *= sign;
	}
	else if (!isdigit(*(a-1)))
	{
	    a = str;
	    goto done;
	}
	else if (*p == 0)
	    goto done;

	if (d == 2.2250738585072011 && e == -308)
	{
	    d = 0.0;
	    a = p;
	    errno = ERANGE;
	    goto done;
	}
	if (d == 2.2250738585072012 && e <= -308)
	{
	    d *= 1.0e-308;
	    a = p;
	    goto done;
	}
	d *= pow(10.0, (double) e);
	a = p;
    }
    else if (p > str && !isdigit(*(p-1)))
    {
	a = str;
	goto done;
    }

done:
    if (end)
	*end = (char*)a;
    return d;
}