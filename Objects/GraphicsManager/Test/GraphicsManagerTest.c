#include "GraphicsManagerTest.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Screen/Vga/VgaDriver.h>
#include <Objects/Common/LinkedList/LinkedList.h>
#include <Objects/GraphicsManager/GraphicsManager.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>
#include <Objects/GraphicsManager/Gui/Image/Image.h>
#include <Objects/GraphicsManager/Gui/Button/Button.h>
#include <Objects/GraphicsManager/Gui/Text/Text.h>
#include <Objects/GraphicsManager/Gui/TextBox/TextBox.h>
#include "../RGB.h"

extern Kernel _Kernel;

void GraphicsManagerTest_RunSuite(GraphicsManager* gm)
{
    //GraphicsManagerTest_LowLevelTest(gm);
    //GraphicsManagerTest_LoadBMP(gm);
    //GraphicsManagerTest_LoadImage(gm);
    GraphicsManagerTest_GuiElements(gm);
}

void GraphicsManagerTest_LowLevelTest(GraphicsManager* gm)
{
    printf("GfxManTest: Low Level Test\n");

    uint8_t n_colors = 8;
    RGB palette_array[8];        
    memset(palette_array,0,sizeof(RGB)*n_colors);

    uint8_t i;
    for (i=0; i<n_colors; i++)
    {
        palette_array[i].Red   = (i & 0x01) == 0x01 ? 255 : 0; 
        palette_array[i].Green = (i & 0x02) == 0x02 ? 255 : 0;
        palette_array[i].Blue  = (i & 0x04) == 0x04 ? 255 : 0;
    }

    printf("Drawing\n");
    int draw_x, draw_y;
    for (draw_y=0; draw_y < _Kernel.Vga.FramebufferHeight; draw_y++)
    {
        for (draw_x=0; draw_x< _Kernel.Vga.FramebufferWidth; draw_x++)
        {
		    VgaDriver_WritePixel(&_Kernel.Vga, draw_x, draw_y, (draw_x + draw_y) % n_colors);
        }
    }
    printf("Drawing done\n");
}

void GraphicsManagerTest_DrawImage(GraphicsManager* gm)
{
    printf("GMT: LoadImage\n");
    char* image_path = "ata0p0://BEACH.BMP";
    Image img;
    Image_Constructor(&img);
    if (Image_LoadFile(&img, image_path))
    {
        GraphicsManager_DrawImage(gm, &img,0,0);
    }
    else
    {
        printf("GMT: Error loading image\n");
    }
    Image_Destructor(&img);
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

void GraphicsManagerTest_GuiElements(GraphicsManager* gm)
{
    printf("GraphicsManagerTest: GuiElements\n");

    printf("GraphicsManagerTest: Adding a button\n");

    Button* button1 = (Button*)malloc(sizeof(Button));
    Button_Constructor(button1);
    button1->GuiElement.Area.Y = 10;
    button1->GuiElement.Area.X = 10;
    Button_SetText(button1,"Button 1");
    GraphicsManager_AddGuiElement(gm, (GuiElement*)button1);

    Button* button2 = (Button*)malloc(sizeof(Button));
    Button_Constructor(button2);
    button2->GuiElement.Area.Y = 40;
    button2->GuiElement.Area.X = 10;
    Button_SetText(button2,"Button 2");
    GraphicsManager_AddGuiElement(gm, (GuiElement*)button2);

    Button* button3 = (Button*)malloc(sizeof(Button));
    Button_Constructor(button3);
    button3->GuiElement.Area.Y = 70;
    button3->GuiElement.Area.X = 10;
    Button_SetText(button3,"Button 3");
    GraphicsManager_AddGuiElement(gm, (GuiElement*)button3);

    Text* text1 = (Text*)malloc(sizeof(Text));
    Text_Constructor(text1);
    text1->GuiElement.Area.Y = 200;
    text1->GuiElement.Area.X = 100;
    Text_SetText(text1, "Test Text");
    GraphicsManager_AddGuiElement(gm, (GuiElement*)text1);

    TextBox* textbox1 = (TextBox*)malloc(sizeof(TextBox));
    TextBox_Constructor(textbox1);
    textbox1->GuiElement.Area.Y = 200;
    textbox1->GuiElement.Area.X = 200;
    TextBox_SetText(textbox1, "Test Text");
    GraphicsManager_AddGuiElement(gm, (GuiElement*)textbox1);

}