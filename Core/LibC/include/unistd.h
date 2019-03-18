#pragma once
#include "stdint.h"
#include "stdio.h"

static inline void sleep(uint32_t seconds)
{
    // stupid sleep impl
    //printf("Sleeping for %d sec\n",seconds);
    unsigned int i;
    for (i=0; i< 150000000; i++)
    {
        asm ("nop");
    }
}