#include "GraphicsManagerTest.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Screen/Vga/VgaDriver.h>
#include <Drivers/Screen/Vga/VgaColorRGB.h>
#include <Objects/Common/LinkedList/LinkedList.h>
#include "../GraphicsManager.h"
#include <Objects/HardwareManager/StorageManager/SMDirectoryEntry.h>
#include "../Image/Image.h"

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
    Image img;
    Image_Constructor(&img);
    if (Image_LoadFile(&img, image_path))
    {
        VgaDriver_SetScreenMode(&_Kernel.Vga,VGA_MODE_PX_640_480_16);
        GraphicsManagerTest_WriteImagePalette(&img);
        VgaDriver_ClearScreen(&_Kernel.Vga);
        Image_Draw(&img,0,0);
    }
    Image_Destructor(&img);
}


uint32_t GraphicsManagerTest_WriteImagePalette(Image* image)
{
    int size = LinkedList_Size(&image->Palette);
    int i;

    for (i=0; i<size; i++)
    {
        if (i >= 16) break;
        VgaColorRGB* data = (VgaColorRGB*)LinkedList_At(&image->Palette, i);
        VgaDriver_WriteDacColor(&_Kernel.Vga, i, *data);
    }
    return LinkedList_Size(&image->Palette);
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