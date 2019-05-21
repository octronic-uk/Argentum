#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "Area.h"

#include <Drivers/PS2/Event.h> 
typedef struct GraphicsManager GraphicsManager;

struct GuiElement
{
    Area Area;
    bool NeedsRedraw;

    bool MouseInside;
    bool MouseInsideLast;

    void (*UpdateFunction)(struct GuiElement* self, GraphicsManager* gm);
    void (*DrawFunction)(struct GuiElement* self, GraphicsManager* gm);

    void (*OnMouseEnterFunction)(struct GuiElement* self, GraphicsManager* gm);
    void (*OnMouseExitFunction)(struct GuiElement* self, GraphicsManager* gm);

    void (*OnMouseClickFunction)(struct GuiElement* self, GraphicsManager* gm);
    void (*OnMouseReleaseFunction)(struct GuiElement* self, GraphicsManager* gm);
};

typedef struct GuiElement GuiElement;

bool GuiElement_Constructor(GuiElement* self);
void GuiElement_Destructor(GuiElement* self);

bool GuiElement_IsMouseOver(GuiElement* self, uint32_t pointer_x, uint32_t pointer_y);