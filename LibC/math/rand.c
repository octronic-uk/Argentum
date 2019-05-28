#include <math.h>
#include <stdio.h>

static double rndseed = 1.0;

int rand()
{ 
	rndseed = rndseed*0x8088405+1;
	return ((int)rndseed) & 0x7FFF;
}

int srand(double i)
{
	rndseed = i;
	return rand();
}