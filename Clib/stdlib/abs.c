#include <stdlib.h>
#include <stdio.h>
int abs(int n) 
{

    printf("stdlib: abs\n");
  if (n >= 0) return n;
  else return -n;
}
