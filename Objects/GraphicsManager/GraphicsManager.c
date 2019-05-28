#include "GraphicsManager.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/PS2/PS2Driver.h>
#include "GraphicsManagerConstants.h"
#include "Gui/GuiElement.h"
#include <Drivers/Screen/Vga/VgaFonts.h>
#include <Objects/GraphicsManager/Gui/Image/Image.h>
#include "RGB.h"

extern Kernel _Kernel;

bool GraphicsManager_Constructor(GraphicsManager* self)
{
    printf("GraphicsManager: Constructor\n");
    memset(self,0,sizeof(GraphicsManager));
    self->Debug = false;

    uint32_t fb_size = sizeof(PixelState)*(_Kernel.Vga.FramebufferWidth * _Kernel.Vga.FramebufferHeight);
    self->Framebuffer = malloc(fb_size);
    if (!self->Framebuffer) 
    {
        printf("GraphicsManager: Could not allocate Framebuffer\n");
        return false;
    }
    memset(self->Framebuffer,0,fb_size);

    LinkedList_Constructor(&self->GuiElements);

    if(GraphicsManager_LoadBackgroundImage(self))
    {
        GraphicsManager_DrawBackgroundImage(self);
    }

    GraphicsManager_LoadCursorImage(self);

    return true;
}

void GraphicsManager_Destructor(GraphicsManager* self)
{
    printf("GraphicsManager: Destructor\n");
    LinkedList_Destructor(&self->GuiElements);
    if (self->Framebuffer)
    {
        free(self->Framebuffer);
    }

    Image_Destructor(&self->BackgroundImage);
    Image_Destructor(&self->CursorImage);
}

bool GraphicsManager_HasMouseStateChanged(GraphicsManager* self)
{
    return self->CurrentMouseEvent.X         != self->LastMouseEvent.X         || 
           self->CurrentMouseEvent.Y         != self->LastMouseEvent.Y         ||
           self->CurrentMouseEvent.Button[0] != self->LastMouseEvent.Button[0] ||
           self->CurrentMouseEvent.Button[1] != self->LastMouseEvent.Button[1] ||
           self->CurrentMouseEvent.Button[2] != self->LastMouseEvent.Button[2];
}

void GraphicsManager_Render(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: Render\n");
    uint32_t n_elements = LinkedList_Size(&self->GuiElements);
    uint32_t i;
    bool pointer_moved = GraphicsManager_HasMouseStateChanged(self);
    GraphicsManager_ClearCursor(self);
    for (i=0; i<n_elements; i++)
    {
        GuiElement* element = (GuiElement*)LinkedList_At(&self->GuiElements, i);

        // Mouse has exited?
        if (element->MouseInside)
        {
            bool mouse_over_in_current_event = GuiElement_IsMouseOver(element, self->CurrentMouseEvent.X, self->CurrentMouseEvent.Y);
            if (!mouse_over_in_current_event)
            {
                if (element->OnMouseExitFunction)
                {
                    element->OnMouseExitFunction(element, self);
                }
            }
        }
        // Check if damaged
        if (element->UpdateFunction) element->UpdateFunction(element, self);  
        if (element->DrawFunction)   element->DrawFunction(element,self);
    }
	GraphicsManager_DrawCursor(self);
    GraphicsManager_Flush(self);
}

void GraphicsManager_DrawText(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t color, char* text)
{
    if (self->Debug) printf("GraphicsManager: DrawText\n");
    char* text_pos = text;
    int char_offset, draw_x, draw_y, mask, line, font_index;
    while (*text_pos)
    {
        font_index = ((*text_pos)*VGA_FONT_8x16_H);
        for (draw_y = y, line = 0; draw_y < y+VGA_FONT_8x16_H; draw_y++, line++)
        {
            for (draw_x = x, mask = 7; draw_x < x+VGA_FONT_8x16_W; draw_x++, mask--)
            {
                char font_bits = VgaFont8x16[font_index + line];    
                if (((font_bits >> mask) & 0x01) != 0)
                {
                    GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, color);
                }
            }
        }
        x += VGA_FONT_8x16_W+1;
        text_pos++;
    }
}

void GraphicsManager_AddGuiElement(GraphicsManager* self, GuiElement* element)
{
    LinkedList_PushBack(&self->GuiElements, element);
}

void GraphicsManager_RemoveGuiElement(GraphicsManager* self, GuiElement* element)
{
    LinkedList_Delete(&self->GuiElements, element);
}

/*
void GraphicsManager_ClearCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: Clear Cursor\n");

    int x, y, draw_x, draw_y, max_x, max_y;
    x = self->LastMouseEvent.X;
    y = self->LastMouseEvent.Y;

    // Edge clamp cursor
    x < _Kernel.Vga.FramebufferWidth - CURSOR_W ? x : _Kernel.Vga.FramebufferWidth - CURSOR_W;
    y < _Kernel.Vga.FramebufferHeight - CURSOR_H ? y : _Kernel.Vga.FramebufferHeight - CURSOR_H;

    max_x = x+ CURSOR_W;
    max_y = y+ CURSOR_H;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
            RGB* bg = (RGB*)self->BackgroundImage.ImageData;
            uint32_t color = RGB_ToUnit32(bg[(draw_y*self->BackgroundImage.Width)+draw_x]);
            GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, color);
        }
    }
}
*/

void GraphicsManager_ClearCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: Clear Cursor\n");

    int x, y, draw_x, draw_y, max_x, max_y;
    x = self->LastMouseEvent.X;
    y = self->LastMouseEvent.Y;

    // Edge clamp cursor
    x < _Kernel.Vga.FramebufferWidth - self->CursorImage.Width ? x : _Kernel.Vga.FramebufferWidth - self->CursorImage.Width;
    y < _Kernel.Vga.FramebufferHeight - self->CursorImage.Height ? y : _Kernel.Vga.FramebufferHeight - self->CursorImage.Height;

    max_x = x+ self->CursorImage.Width;
    max_y = y+ self->CursorImage.Height;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
            RGB* bg = (RGB*)self->BackgroundImage.ImageData;
            uint32_t color = RGB_ToUnit32(bg[(draw_y*self->BackgroundImage.Width)+draw_x]);
            GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, color);
        }
    }
}
/*
void GraphicsManager_DrawCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: Render Cursor\n");

    int x, y, draw_x, draw_y, max_x, max_y, line, mask;
    x = self->CurrentMouseEvent.X;
    y = self->CurrentMouseEvent.Y;

    // Edge clamp cursor
    x < _Kernel.Vga.FramebufferWidth - CURSOR_W ? x : _Kernel.Vga.FramebufferWidth - CURSOR_W;
    y < _Kernel.Vga.FramebufferHeight - CURSOR_H ? y : _Kernel.Vga.FramebufferHeight - CURSOR_H;

    max_x = self->CurrentMouseEvent.X+CURSOR_W;
    max_y = self->CurrentMouseEvent.Y+CURSOR_H;
    
    for(draw_y = y, line=0; draw_y < max_y; draw_y++, line++)
    {
        for (draw_x = x, mask=7; draw_x < max_x; draw_x++, mask--)
        {
            if (((PointerData[line]>>mask) & 0x01) == 0x01)
            {
                GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, DEFAULT_PALETTE_WHITE);
            }
        }
    }
    self->LastMouseEvent = self->CurrentMouseEvent;
}
*/

void GraphicsManager_DrawCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: Render Cursor\n");

    int x, y, draw_x, draw_y, max_x, max_y;
    x = self->CurrentMouseEvent.X;
    y = self->CurrentMouseEvent.Y;

    // Edge clamp cursor
    x < _Kernel.Vga.FramebufferWidth - self->CursorImage.Width ? x : _Kernel.Vga.FramebufferWidth - self->CursorImage.Width;
    y < _Kernel.Vga.FramebufferHeight - self->CursorImage.Height ? y : _Kernel.Vga.FramebufferHeight - self->CursorImage.Height;

    max_x = self->CurrentMouseEvent.X + self->CursorImage.Width;
    max_y = self->CurrentMouseEvent.Y + self->CursorImage.Height;
    
    uint32_t img_x, img_y;
    for(draw_y = y, img_y=0; draw_y < max_y; draw_y++, img_y++)
    {
        for (draw_x = x, img_x=0; draw_x < max_x; draw_x++, img_x++)
        {
            RGB* data_as_rgb = &((RGB*)self->CursorImage.ImageData)[(img_y*self->CursorImage.Width)+img_x];
            if (data_as_rgb->Red == 0xFF && data_as_rgb->Green == 0x00 && data_as_rgb->Blue == 0xFF)
            {

            }
            else
            {
                uint32_t u = RGB_ToUnit32(*data_as_rgb);
                GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y,u);
            }
        }
    }
    self->LastMouseEvent = self->CurrentMouseEvent;
}

void GraphicsManager_WritePixelToFramebuffer(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t color)
{
    if (self->Debug) printf("GraphicsManager: WritePixelToFramebuffer\n");
    PixelState current = self->Framebuffer[(y*_Kernel.Vga.FramebufferWidth)+x]; 

    if (current.Color != color)
    {
        self->Framebuffer[(y*_Kernel.Vga.FramebufferWidth)+x].Color = color;
        self->Framebuffer[(y*_Kernel.Vga.FramebufferWidth)+x].Damaged = true;
    }
}

void GraphicsManager_DrawRect(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    if (self->Debug) printf("GraphicsManager: DrawRect\n");
    int draw_x, draw_y, max_x, max_y;
    max_y = y+h;
    max_x = x+w;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
           GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, color);
        }
    }
}

void GraphicsManager_DrawLine(GraphicsManager* self, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
    if (self->Debug) printf("GraphicsManager: \n");
}

void GraphicsManager_Flush(GraphicsManager* self)
{
    if(self->Debug) printf("GraphicsManager: Flush\n");

    uint32_t x, y;

    for (y = 0; y < _Kernel.Vga.FramebufferHeight; y++)
    {
        for (x = 0; x < _Kernel.Vga.FramebufferWidth; x++)
        {
            PixelState *s = &self->Framebuffer[(y*_Kernel.Vga.FramebufferWidth)+x];
            if (s->Damaged)
            {
                VgaDriver_WritePixel(&_Kernel.Vga, x, y, s->Color);
                s->Damaged = false;
            }
        }
    }
}

GuiElement* GraphicsManager_GetElementAt(GraphicsManager* self, uint32_t x, uint32_t y)
{
    uint32_t i, n_elements;
    n_elements = LinkedList_Size(&self->GuiElements);
    for (i=0; i<n_elements; i++)
    {
        GuiElement* e = (GuiElement*)LinkedList_At(&self->GuiElements, i);
        if (GuiElement_IsMouseOver(e, x,y))
        {
            return e;
        }
    }
    return 0;
}

void GraphicsManager_KeyboardEventToActiveGuiElement(GraphicsManager* self, KeyboardEvent e)
{
    if (self->FocusedGuiElement)
    {
        if (self->FocusedGuiElement->OnKeyboardEventFunction)
        {
            self->FocusedGuiElement->OnKeyboardEventFunction(self->FocusedGuiElement, self, e);
        }
    }
}

void GraphicsManager_MouseEventToActiveGuiElement(GraphicsManager* self, MouseEvent e)
{
    GuiElement* element = GraphicsManager_GetElementAt(self,e.X, e.Y);
    if (e.Button[0])
    {
        self->FocusedGuiElement = element;
    }
    if (element)
    {
        // Mouse Entered
        if (!element->MouseInside)
        {
            bool mouse_over_in_last_event = GuiElement_IsMouseOver(element, self->LastMouseEvent.X, self->LastMouseEvent.Y);
            if (!mouse_over_in_last_event)
            {
                if (element->OnMouseEnterFunction)
                {
                    element->OnMouseEnterFunction(element, self);
                }
            }
        }
        // Mouse Exit handled in Update Loop

        // Mouse click / release
        if (!self->LastMouseEvent.Button[0] && e.Button[0])
        {
            if (element->OnMouseClickFunction)
            {
                element->OnMouseClickFunction(element,self);
            }
        }
        else if (self->LastMouseEvent.Button[0] && !e.Button[0])
        {
            if (element->OnMouseReleaseFunction)
            {
                element->OnMouseReleaseFunction(element,self);
            }
        }
    }
}

void GraphicsManager_DrawImage(GraphicsManager* self, Image* image, uint32_t at_x, uint32_t at_y)
{
    if (image->ImageData == 0) return;

    printf("Image: Drawing starts\n");
    int x,y;
    for(y=0; y < image->Height; y++)
    {
        for (x=0; x < image->Width; x++)
        {
            RGB* data_as_rgb = &((RGB*)image->ImageData)[(y*image->Width)+x];
            uint32_t u = RGB_ToUnit32(*data_as_rgb);
            VgaDriver_WritePixel(&_Kernel.Vga, x, y, u);
        }
    }
    printf("Image: Drawing done\n");
}

bool GraphicsManager_LoadBackgroundImage(GraphicsManager* self)
{
    if(self->Debug) printf("GraphicsManager: Loading Background Image\n");
    char* image_path = "ata0p0://SYS/BG/BLUR.BMP";
    Image_Constructor(&self->BackgroundImage);
    if (Image_LoadFile(&self->BackgroundImage, image_path))
    {
        return true;
    }
    else
    {
        printf("GraphicsManager: Error loading image\n");
    }
    return false;
}

bool GraphicsManager_LoadCursorImage(GraphicsManager* self)
{
    if(self->Debug) printf("GraphicsManager: Loading Cursor Image\n");
    char* image_path = "ata0p0://SYS/CURSOR/CUR1.BMP";
    Image_Constructor(&self->CursorImage);
    if (Image_LoadFile(&self->CursorImage, image_path))
    {
        return true;
    }
    else
    {
        printf("GraphicsManager: Error loading cursor image\n");
    }
    return false;
}

void GraphicsManager_DrawBackgroundImage(GraphicsManager* self)
{
    GraphicsManager_DrawImage(self, &self->BackgroundImage,0,0);
}


void GraphicsManager_ReplaceBackgroundArea(GraphicsManager* self, Area area)
{
    if (self->Debug) printf("GraphicsManager: Replacing Background Area\n");

    int x, y, draw_x, draw_y, max_x, max_y;
    x = area.X;
    y = area.Y;

    max_x = x + area.W;
    max_y = y + area.H;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
            RGB* bg = (RGB*)self->BackgroundImage.ImageData;
            uint32_t color = RGB_ToUnit32(bg[(draw_y*self->BackgroundImage.Width)+draw_x]);
            GraphicsManager_WritePixelToFramebuffer(self, draw_x, draw_y, color);
        }
    }
}