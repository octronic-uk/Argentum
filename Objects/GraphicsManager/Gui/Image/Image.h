#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/Common/LinkedList/LinkedList.h>

struct Image
{
    int Width;
    int Height;
    int BytesPerPixel;
    uint8_t* ImageData;
};
typedef struct Image Image;

bool Image_Constructor(Image* self);
void Image_Destructor(Image* self);

bool Image_LoadFile(Image* self, char* image_path);