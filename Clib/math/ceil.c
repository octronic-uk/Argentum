#include <math.h>

double ceil(double i)
{
	if (i<0)
		return -floor(-i);
	else
	{
		double d = floor(i);
		if (d!=i)
			d+=1.0;
		return d;
	}
}