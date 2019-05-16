#include "GraphicsManagerTest.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Objects/GraphicsManager/GraphicsManager.h>
#include <Drivers/Screen/Vga/VgaDriver.h>
#include <Drivers/Screen/Vga/VgaColorRGB.h>
#include <Objects/LinkedList/LinkedList.h>
#include <Objects/GraphicsManager/BitmapFile/BitmapFile.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>

#define STB_IMAGE_IMPLEMENTATION 1
#include "../stb_image.h"

extern Kernel _Kernel;

void GraphicsManagerTest_RunSuite(GraphicsManager* gm)
{
    GraphicsManagerTest_LowLevelTest(gm);
    GraphicsManagerTest_LoadBMP(gm);
}

void GraphicsManagerTest_LowLevelTest(GraphicsManager* gm)
{
    printf("GfxManTest: Low Level Test\n");
    VgaDriver_SetScreenMode(&_Kernel.Vga,VGA_MODE_PX_640_480_16);

    uint8_t n_colors = 8;
    VgaColorRGB palette_array[8];        
    memset(palette_array,0,sizeof(VgaColorRGB)*n_colors);

    uint8_t i;
    for (i=0; i<n_colors; i++)
    {
        palette_array[i].Red   = (i & 0x01) == 0x01 ? 255 : 0; 
        palette_array[i].Green = (i & 0x02) == 0x02 ? 255 : 0;
        palette_array[i].Blue  = (i & 0x04) == 0x04 ? 255 : 0;
    }
    VgaDriver_WriteColorPalette(&_Kernel.Vga, palette_array, n_colors);

    printf("Drawing\n");
    int draw_x, draw_y;
    for (draw_y=0; draw_y<480; draw_y++)
    {
        for (draw_x=0; draw_x<640; draw_x++)
        {
		    _Kernel.Vga.WritePixelFunction(&_Kernel.Vga, draw_x, draw_y, (draw_x + draw_y) % n_colors);
        }
    }
    printf("Drawing done\n");
}

void GraphicsManagerTest_LoadBMP(GraphicsManager* gm)
{
    printf("GMT: LoadBMP\n");
    char* image_path = "ata0p0://LAND3.BMP";

    FILE* f = fopen(image_path,"rb");

    if (!f)
    {
        printf("Image File Load Error: %u\n");
        return;
    }

    int image_width, image_height, image_bpp;
    uint8_t* image_data = stbi_load_from_file(f, &image_width, &image_height, &image_bpp, STBI_rgb);
    printf("STB Image witdh %d height %d bpp %d\n",image_width, image_height, image_bpp);
    LinkedList palette;
    LinkedList_Constructor(&palette);
    VgaColorRGB* data_as_rgb = (VgaColorRGB*)image_data;
    uint32_t palette_size = GraphicsManagerTest_CreatePalette(&palette, (VgaColorRGB*)image_data, image_width*image_height);
    printf("Found %d colors in palette\n",palette_size);

    if (palette_size != 8)
    {
        printf("Incorrect palette size\n",palette_size);
        return;
    }

    VgaDriver_SetScreenMode(&_Kernel.Vga,VGA_MODE_PX_640_480_16);
	VgaDriver_ClearScreen(&_Kernel.Vga);
    printf("Drawing starts\n");
    int x,y;
    for(y=0; y<image_height; y++)
    {
        for (x=0; x<image_width; x++)
        {
            uint32_t pixel_index = (y * image_width) + x;
            uint8_t palette_index = GraphicsManagerTest_PaletteIndex(&palette, &data_as_rgb[pixel_index]);
            _Kernel.Vga.WritePixelFunction(&_Kernel.Vga, x, y, palette_index);
        }
    }
    printf("Drawing done\n");
    LinkedList_Destructor(&palette);
    stbi_image_free(image_data);
    image_data = 0;
    data_as_rgb = 0;
}


uint32_t GraphicsManagerTest_CreatePalette(LinkedList* palette, VgaColorRGB* data, uint32_t size)
{
    int i, p_index=0;
    for (i=0; i<size; i++)
    {
        if (p_index >= 16) break;
        if (!GraphicsManagerTest_InPalette(palette, &data[i]))
        {
            LinkedList_PushBack(palette, &data[i]);
            VgaDriver_WriteDacColor(&_Kernel.Vga, p_index, data[i]);
            p_index++;
        }
    }
    return LinkedList_Size(palette);
}

uint8_t GraphicsManagerTest_PaletteIndex(LinkedList* palette, VgaColorRGB* rgb)
{
    uint8_t sz = LinkedList_Size(palette);
    uint8_t i;
    for (i=0; i<sz; i++)
    {
        VgaColorRGB* next = (VgaColorRGB*)LinkedList_At(palette,i);
        if (VgaColorRGB_EqualTo(*rgb, *next))
        {
            return i;
        }
    }
    abort();
    return 0;
}

bool GraphicsManagerTest_InPalette(LinkedList* palette, VgaColorRGB* rgb)
{
    int sz = LinkedList_Size(palette);
    int i;
    for (i=0; i<sz; i++)
    {
        VgaColorRGB* next = LinkedList_At(palette,i);
        if (VgaColorRGB_EqualTo(*rgb, *next))
        {
            return true;
        }
    }
    return false;
}

void GraphicsManagerTest_DebugData(uint8_t* data, uint32_t size)
{
    int i;
    for (i=0; i<size; i++)
    {
		if (i % 512 == 0)
		{
			printf("\n\n        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F\n");
			    printf("        -----------------------------------------------------------------------------------------------");
		}
        if (i % 32 == 0)
        {
            printf("\n0x%04x: ",i);
        }
        printf("%02x ",data[i]);
    }
	printf("\n");
}


void GraphicsManagerTest_DebugDataUnformatted(uint8_t* data, uint32_t size)
{
    int i;
    for (i=0; i<size; i++)
    {
        if (i % 32 == 0)
        {
            printf("\n");
        }
        printf("%02x ",data[i]);
    }
	printf("\n");
}