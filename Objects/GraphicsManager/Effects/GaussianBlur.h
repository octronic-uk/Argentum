#pragma once

#include <stdbool.h>
#include <stdint.h>

struct GaussianBlur
{
    bool Debug;
    double Kernel[5][5];
};

typedef struct GaussianBlur GaussianBlur;

bool GaussianBlur_Constructor(GaussianBlur* self);
void GaussianBlur_Destructor(GaussianBlur* self);
void GaussianBlur_CreateFilter(GaussianBlur* self);