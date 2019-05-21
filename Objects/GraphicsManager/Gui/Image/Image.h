#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/Common/LinkedList/LinkedList.h>

typedef struct VgaColorRGB VgaColorRGB;

struct Image
{
    int Width;
    int Height;
    int BytesPerPixel;
    uint8_t* ImageData;
    LinkedList Palette;
};
typedef struct Image Image;

bool Image_Constructor(Image* self);
void Image_Destructor(Image* self);

void Image_Draw(Image* self, uint32_t at_x, uint32_t at_y);
bool Image_LoadFile(Image* self, char* image_path);
uint32_t Image_CreatePalette(Image* self);
int8_t Image_PaletteIndex(Image* self, VgaColorRGB* rgb);
bool Image_InPalette(Image* self, VgaColorRGB* rgb);