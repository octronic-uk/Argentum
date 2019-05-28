#include "Image.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

bool Image_Constructor(Image* self)
{
    printf("Image: Constructing\n");
    memset(self, 0, sizeof(Image));
    return true;
}

bool Image_LoadFile(Image* self, char* image_path)
{
    printf("Image: LoadFile\n");

    FILE* f = fopen(image_path,"rb");

    if (!f)
    {
        printf("Image: File Load Error: %u\n");
        return false;
    }

    self->ImageData = stbi_load_from_file(f, &self->Width, &self->Height, &self->BytesPerPixel, STBI_rgb);
    printf("Image: STB width %d height %d bpp %d\n", self->Width, self->Height, self->BytesPerPixel);

    return self->ImageData != 0;
}

void Image_Destructor(Image* self)
{
    printf("Image: Destructing\n");
    if (self->ImageData != 0) stbi_image_free((uint8_t*)self->ImageData);
    self->ImageData = 0;
}