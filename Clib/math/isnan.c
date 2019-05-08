#include <math.h>

int __isnan(double x)
{
	int32_t hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (uint32_t)(lx|(-lx))>>31;
	hx = 0x7ff00000 - hx;
	return (int)(((uint32_t)hx)>>31);
}

int __isnanf(float x)
{
	int32_t ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;
	ix = 0x7f800000 - ix;
	return (int)(((uint32_t)(ix))>>31);
}

int __isnanl (long double value)
{
    // TODO - bit of a copout
  return 0;
}