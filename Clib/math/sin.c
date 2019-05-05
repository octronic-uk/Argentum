#include <math.h>

double sin(double i)
{
	int n,k;
	double j,sum;
	int sign = 0;

	if (i<0)
	{
		sign = 1;
		i = -i;
	}

	i *= 1.0/(2*PI);
	i -= floor(i);
	if (i>=0.5)
	{
		i -= 0.5;
		sign ^= 1;
	}

	i *= 2*PI;
	k = 1;
	j = sum = i;
	for (n=3;n<14;n+=2)
	{
		k *= (n-1)*n;
		j *= i*i;
		if (n & 2)
			sum -= j/k;
		else
			sum += j/k;
	}

	return sign ? -sum:sum;
}