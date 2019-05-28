#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../GuiElement.h"

#define TEXT_BUFFER_SIZE 128

#define TEXT_DEFAULT_WIDTH  100 
#define TEXT_DEFAULT_HEIGHT 20

struct Text
{
    GuiElement GuiElement;
    bool Debug;
    char Text[TEXT_BUFFER_SIZE];
    uint32_t Color;
};
typedef struct Text Text;

bool Text_Constructor(Text* self);
void Text_Destructor(Text* self);

void Text_SetText(Text* self, char* text);

void Text_OnDraw(GuiElement* self, GraphicsManager* gm);
void Text_OnUpdate(GuiElement* self, GraphicsManager* gm);
void Text_OnMouseEnter(GuiElement* self, GraphicsManager* gm);
void Text_OnMouseExit(GuiElement* self, GraphicsManager* gm);
void Text_OnMouseClick(GuiElement* self, GraphicsManager* gm);
void Text_OnMouseRelease(GuiElement* self, GraphicsManager* gm);
