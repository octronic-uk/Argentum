#include "GraphicsManager.h"

#include <string.h>
#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/PS2/PS2Driver.h>
#include "GraphicsManagerConstants.h"
#include "Gui/GuiElement.h"
#include <Drivers/Screen/Vga/VgaFonts.h>

extern Kernel _Kernel;

bool GraphicsManager_Constructor(GraphicsManager* self)
{
    printf("GraphicsManager: Constructor\n");
    VgaDriver_SetScreenMode(&_Kernel.Vga,VGA_MODE_PX_640_480_16);
    memset(self,0,sizeof(GraphicsManager));
    self->Debug = false;
    uint32_t fb_size = sizeof(PixelState)*(_Kernel.Vga.Width*_Kernel.Vga.Height);
    self->Framebuffer = MemoryDriver_Allocate(&_Kernel.Memory, fb_size);
    memset(self->Framebuffer,0,fb_size);

    if (!self->Framebuffer) 
    {
        printf("GraphicsManager: Could not allocate Framebuffer\n");
        return false;
    }

    LinkedList_Constructor(&self->GuiElements);
    GraphicsManager_SetDefaultPalette(self);
    GraphicsManager_DrawBackground(self);
    return true;
}

void GraphicsManager_Destructor(GraphicsManager* self)
{
    printf("GraphicsManager: Destructor\n");
    LinkedList_Destructor(&self->GuiElements);
    if (self->Framebuffer)
    {
        MemoryDriver_Free(&_Kernel.Memory, self->Framebuffer);
    }
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
    uint32_t n_elements = LinkedList_Size(&self->GuiElements);
    uint32_t i;
    bool pointer_moved = GraphicsManager_HasMouseStateChanged(self);
    //if (pointer_moved) GraphicsManager_ClearCursor(self);
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
        element->UpdateFunction(element, self);  
        // Redraw
        //if (element->NeedsRedraw)
        //{
            if (element->DrawFunction)
                element->DrawFunction(element,self);
            else
                printf("GraphicsManager: Element %d has no draw function\n",i);
        //}
    }
	//if (pointer_moved) GraphicsManager_RenderCursor(self);
	GraphicsManager_RenderCursor(self);
    GraphicsManager_Flush(self);
}


void GraphicsManager_PutText(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color, char* text)
{
    char* text_pos = text;
    int char_offset, draw_x, draw_y, mask, line, font_index;
    while (*text_pos)
    {
        font_index = ((*text_pos)*16);
        for (draw_y = y, line = 0; draw_y < y+16; draw_y++, line++)
        {
            for (draw_x = x, mask = 7; draw_x < x+8; draw_x++, mask--)
            {
                char font_bits = g_8x16_font[font_index + line];    
                if (((font_bits >> mask) & 0x01) != 0)
                {
                    GraphicsManager_PutPixelToFramebuffer(self, draw_x, draw_y, color);
                }
            }
        }
        x += 9;
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

void GraphicsManager_ClearCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: RenderCursor\n");

    int x, y, draw_x, draw_y, max_x, max_y;
    x = self->LastMouseEvent.X;
    y = self->LastMouseEvent.Y;
    max_y = y+16;
    max_x = x+8;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
            GraphicsManager_PutPixelToFramebuffer(self, draw_x, draw_y, PALETTE_INDEX_BACKGROUND_COLOR);
            //self->Framebuffer[(draw_y*_Kernel.Vga.Width)+draw_x].Damaged = true;
        }
    }
}

void GraphicsManager_RenderCursor(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: RenderCursor\n");

    int x, y, draw_x, draw_y, max_x, max_y, line, mask;
    max_x = self->CurrentMouseEvent.X+8;
    max_y = self->CurrentMouseEvent.Y+16;
    
    for(draw_y = self->CurrentMouseEvent.Y, line=0; draw_y < max_y; draw_y++, line++)
    {
        for (draw_x = self->CurrentMouseEvent.X, mask=7; draw_x < max_x; draw_x++, mask--)
        {
            if (((PointerData[line]>>mask) & 0x01) == 0x01)
            {
                GraphicsManager_PutPixelToFramebuffer(self, draw_x, draw_y, 
                    self->CurrentMouseEvent.Button[0] ?  PALETTE_INDEX_BLACK : PALETTE_INDEX_WHITE );
            }
        }
    }
    self->LastMouseEvent = self->CurrentMouseEvent;
}

void GraphicsManager_PutPixelToFramebuffer(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color)
{
    PixelState current = self->Framebuffer[(y*_Kernel.Vga.Width)+x]; 
    if (current.Color != color)
    {
        self->Framebuffer[(y*_Kernel.Vga.Width)+x].Color = color;
        self->Framebuffer[(y*_Kernel.Vga.Width)+x].Damaged = true;
    }
}

void GraphicsManager_PutPixelToScreen(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color)
{
    _Kernel.Vga.WritePixelFunction(&_Kernel.Vga, x, y, color);
}

void GraphicsManager_PutRect(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color)
{
    if (self->Debug) printf("GraphicsManager: PutRect\n");
    int draw_x, draw_y, max_x, max_y;
    max_y = y+h;
    max_x = x+w;

    for(draw_y=y; draw_y < max_y; draw_y++)
    {
        for (draw_x=x; draw_x < max_x; draw_x++)
        {
           GraphicsManager_PutPixelToFramebuffer(self, draw_x, draw_y, color);
        }
    }
}

void GraphicsManager_PutLine(GraphicsManager* self, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color)
{
    if (self->Debug) printf("GraphicsManager: \n");

}

void GraphicsManager_SetDefaultPalette(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: \n");
    VgaDriver_WriteColorPalette(&_Kernel.Vga, DefaultPalette, 16);
}

void GraphicsManager_DrawBackground(GraphicsManager* self)
{
    if (self->Debug) printf("GraphicsManager: \n");
    int draw_x, draw_y, max_x, max_y;

    for(draw_y=0; draw_y < _Kernel.Vga.Height; draw_y++)
    {
        for (draw_x=0; draw_x < _Kernel.Vga.Width; draw_x++)
        {
           GraphicsManager_PutPixelToFramebuffer(self, draw_x, draw_y, PALETTE_INDEX_BACKGROUND_COLOR);
        }
    }
}

void GraphicsManager_Flush(GraphicsManager* self)
{
    uint32_t x, y;

    for (y = 0; y < _Kernel.Vga.Height; y++)
    {
        for (x = 0; x < _Kernel.Vga.Width; x++)
        {
            PixelState *s = &self->Framebuffer[(y*_Kernel.Vga.Width)+x];
            if (s->Damaged)
            {
                GraphicsManager_PutPixelToScreen(self,x,y,s->Color);
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

}

void GraphicsManager_MouseEventToActiveGuiElement(GraphicsManager* self, MouseEvent e)
{
    GuiElement* element = GraphicsManager_GetElementAt(self,e.X, e.Y);
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