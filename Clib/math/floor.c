#include <math.h>

double floor(double i)
{
	if (i<0) 
		return -ceil(-i);
	else
		return (double)(int)i;
}