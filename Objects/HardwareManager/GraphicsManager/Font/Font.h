#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct VgaColorRGB VgaColorRGB;

struct Font
{
};
typedef struct Font Font;

bool Font_Constructor(Font* self);
void Font_Destructor(Font* self);