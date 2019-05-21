#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <Drivers/Screen/Vga/VgaColorRGB.h>
#include <Objects/Common/LinkedList/LinkedList.h>
#include <Drivers/PS2/Event.h>

typedef struct GuiElement GuiElement;

struct PixelState
{
    uint8_t Color;
    bool    Damaged;
};
typedef struct PixelState PixelState;

struct GraphicsManager
{
    bool Debug;
    VgaColorRGB Palette[16];
    PixelState* Framebuffer;
    LinkedList  GuiElements;
    MouseEvent  CurrentMouseEvent;
    MouseEvent  LastMouseEvent;
    KeyboardEvent CurrentKeyboardEvent;
    KeyboardEvent LastKeyboardEvent;
    GuiElement*   ActiveGuiElement;
};

typedef struct GraphicsManager GraphicsManager;

bool GraphicsManager_Constructor(GraphicsManager* self);
void GraphicsManager_Destructor(GraphicsManager* self);

// Drawing
void GraphicsManager_Render(GraphicsManager* self);
void GraphicsManager_PutPixelToScreen(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color);
void GraphicsManager_PutPixelToFramebuffer(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color);
void GraphicsManager_PutRect(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color);
void GraphicsManager_PutLine(GraphicsManager* self, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t color);
void GraphicsManager_PutText(GraphicsManager* self, uint32_t x, uint32_t y, uint8_t color, char* text);
void GraphicsManager_DrawBackground(GraphicsManager* self);
void GraphicsManager_SetDefaultPalette(GraphicsManager* self);
void GraphicsManager_Flush(GraphicsManager* self);

void GraphicsManager_ClearCursor(GraphicsManager* self);
void GraphicsManager_RenderCursor(GraphicsManager* self);

// Misc
bool GraphicsManager_HasMouseStateChanged(GraphicsManager* self);

// Gui Elements
void GraphicsManager_AddGuiElement(GraphicsManager* self, GuiElement* element);
void GraphicsManager_RemoveGuiElement(GraphicsManager* self, GuiElement* element);
GuiElement* GraphicsManager_GetElementAt(GraphicsManager* self, uint32_t x, uint32_t y);

void GraphicsManager_KeyboardEventToActiveGuiElement(GraphicsManager* self, KeyboardEvent e);
void GraphicsManager_MouseEventToActiveGuiElement(GraphicsManager* self, MouseEvent e);