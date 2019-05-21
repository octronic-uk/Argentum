#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Area
{
    uint32_t X;
    uint32_t Y;
    uint32_t W;
    uint32_t H;
};

typedef struct Area Area;

bool Area_IsMouseOver(Area* self, uint32_t pointer_x, uint32_t pointer_y);