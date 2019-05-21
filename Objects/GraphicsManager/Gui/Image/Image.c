#include "Image.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"
#include <Drivers/Screen/Vga/VgaColorRGB.h>
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
    printf("Image: STB witdh %d height %d bpp %d\n", self->Width, self->Height, self->BytesPerPixel);
    LinkedList_Constructor(&self->Palette);

    uint32_t palette_size = Image_CreatePalette(self);
    printf("Image: Found %d colors in palette\n",palette_size);

    if (palette_size > 16)
    {
        printf("Image: Palette is too large\n",palette_size);
        return false;
    }
    return true;
}

void Image_Draw(Image* self, uint32_t at_x, uint32_t at_y)
{
    printf("Image: Drawing starts\n");
    VgaColorRGB* data_as_rgb = (VgaColorRGB*)self->ImageData;
    int x,y;
    for(y=0; y < self->Height; y++)
    {
        for (x=0; x < self->Width; x++)
        {
            uint32_t pixel_index = (y * self->Width) + x;
            uint8_t palette_index = Image_PaletteIndex(self, &data_as_rgb[pixel_index]);
            if (palette_index > -1)
            {
                _Kernel.Vga.WritePixelFunction(&_Kernel.Vga, x, y, palette_index);
            }
        }
    }
    printf("Drawing done\n");
}

void Image_Destructor(Image* self)
{
    printf("Image: Destructing\n");
    LinkedList_Destructor(&self->Palette);
    if (self->ImageData != 0) stbi_image_free((uint8_t*)self->ImageData);
    self->ImageData = 0;
}

uint32_t Image_CreatePalette(Image* self)
{
    VgaColorRGB* data = (VgaColorRGB*)self->ImageData;
    int size=self->Width*self->Height, i, p_index=0;
    for (i=0; i<size; i++)
    {
        if (p_index >= 16) break;
        if (!Image_InPalette(self, &data[i]))
        {
            LinkedList_PushBack(&self->Palette, &data[i]);
            p_index++;
        }
    }
    return LinkedList_Size(&self->Palette);
}

int8_t Image_PaletteIndex(Image* self, VgaColorRGB* rgb)
{
    uint8_t sz = LinkedList_Size(&self->Palette);
    uint8_t i;
    for (i=0; i<sz; i++)
    {
        VgaColorRGB* next = (VgaColorRGB*)LinkedList_At(&self->Palette,i);
        if (VgaColorRGB_EqualTo(*rgb, *next))
        {
            return i;
        }
    }
    return -1;
}

bool Image_InPalette(Image* self, VgaColorRGB* rgb)
{
    return Image_PaletteIndex(self, rgb) > -1;
}