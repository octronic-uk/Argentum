#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../GuiElement.h"

#define BUTTON_TEXT_BUFFER_SIZE 32
#define BUTTON_DEFAULT_WIDTH  100 
#define BUTTON_DEFAULT_HEIGHT 20 

struct Button
{
    GuiElement GuiElement;
    bool Debug;
    char Text[BUTTON_TEXT_BUFFER_SIZE];
    uint32_t Color;
    uint32_t HoverColor;
};
typedef struct Button Button;

bool Button_Constructor(Button* self);
void Button_Destructor(Button* self);

void Button_SetText(Button* self, char* text);
void Button_OnDraw(GuiElement* self, GraphicsManager* gm);
void Button_OnUpdate(GuiElement* self, GraphicsManager* gm);
void Button_OnMouseEnter(GuiElement* self, GraphicsManager* gm);
void Button_OnMouseExit(GuiElement* self, GraphicsManager* gm);
void Button_OnMouseClick(GuiElement* self, GraphicsManager* gm);
void Button_OnMouseRelease(GuiElement* self, GraphicsManager* gm);
