#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../GuiElement.h"

#define BUTTON_TEXT_SIZE 32

struct Button
{
    GuiElement GuiElement;
    bool Debug;
    char Text[BUTTON_TEXT_SIZE];
    uint8_t ForegroundColor;
    uint8_t BackgroundColor;
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
