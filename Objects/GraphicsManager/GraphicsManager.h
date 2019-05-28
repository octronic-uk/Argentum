#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <Objects/Common/LinkedList/LinkedList.h>
#include <Drivers/PS2/Event.h>
#include "Gui/Image/Image.h"
#include "Gui/Area.h"

typedef struct GuiElement GuiElement;

struct PixelState
{
    uint32_t Color;
    bool    Damaged;
};
typedef struct PixelState PixelState;

struct GraphicsManager
{
    bool Debug;
    PixelState*   Framebuffer;
    LinkedList    GuiElements;
    MouseEvent    CurrentMouseEvent;
    MouseEvent    LastMouseEvent;
    KeyboardEvent CurrentKeyboardEvent;
    KeyboardEvent LastKeyboardEvent;
    // On Screen Elements
    Image BackgroundImage;
    Image CursorImage;
    GuiElement*   FocusedGuiElement;
};

typedef struct GraphicsManager GraphicsManager;

bool GraphicsManager_Constructor(GraphicsManager* self);
void GraphicsManager_Destructor(GraphicsManager* self);

// Drawing
void GraphicsManager_Render(GraphicsManager* self);
void GraphicsManager_WritePixelToFramebuffer(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t color);
void GraphicsManager_DrawRect(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void GraphicsManager_DrawLine(GraphicsManager* self, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void GraphicsManager_DrawText(GraphicsManager* self, uint32_t x, uint32_t y, uint32_t color, char* text);
void GraphicsManager_DrawImage(GraphicsManager* self, Image* image, uint32_t at_x, uint32_t at_y);
void GraphicsManager_Flush(GraphicsManager* self);
void GraphicsManager_ClearCursor(GraphicsManager* self);
void GraphicsManager_DrawCursor(GraphicsManager* self);
void GraphicsManager_DrawBackgroundImage(GraphicsManager* self);
void GraphicsManager_ReplaceBackgroundArea(GraphicsManager* self, Area area);

bool GraphicsManager_LoadBackgroundImage(GraphicsManager* self);
bool GraphicsManager_LoadCursorImage(GraphicsManager* self);

// Gui Elements
void GraphicsManager_AddGuiElement(GraphicsManager* self, GuiElement* element);
void GraphicsManager_RemoveGuiElement(GraphicsManager* self, GuiElement* element);
GuiElement* GraphicsManager_GetElementAt(GraphicsManager* self, uint32_t x, uint32_t y);
void GraphicsManager_KeyboardEventToActiveGuiElement(GraphicsManager* self, KeyboardEvent e);
void GraphicsManager_MouseEventToActiveGuiElement(GraphicsManager* self, MouseEvent e);
bool GraphicsManager_HasMouseStateChanged(GraphicsManager* self);
