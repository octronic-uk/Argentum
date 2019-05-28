#include "GaussianBlur.h"

/*
    Inspired by: https://www.programming-techniques.com/2013/02/gaussian-filter-generation-using-cc.html
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

bool GaussianBlur_Constructor(GaussianBlur* self)
{
    printf("GaussianBlur: Constructor\n");
    memset(self,0,sizeof(GaussianBlur));

    GaussianBlur_CreateFilter(self);

    if (self->Debug)
    {
        for(int i = 0; i < 5; ++i)
        {
            for (int j = 0; j < 5; ++j)
            {
                printf("%d\t", self->Kernel[i][j]);;
            }
            printf("\n");
        }
    }

    return true;
}

void GaussianBlur_Destructor(GaussianBlur* self)
{
    
}

void GaussianBlur_CreateFilter(GaussianBlur* self)
{
    // set standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;
 
    // sum is for normalization
    double sum = 0.0;
 
    // generate 5x5 kernel
    for (int x = -2; x <= 2; x++)
    {
        for(int y = -2; y <= 2; y++)
        {
            r = sqrt(x*x + y*y);
            self->Kernel[x + 2][y + 2] = (exp(-(r*r)/s))/(M_PI * s);
            sum += self->Kernel[x + 2][y + 2];
        }
    }
 
    // normalize the Kernel
    for(int i = 0; i < 5; ++i)
    {
        for(int j = 0; j < 5; ++j)
        {
            self->Kernel[i][j] /= sum;
        }
    }
}